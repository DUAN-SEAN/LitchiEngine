
// = INCLUDES ========
#include "../Common/common.hlsl"
//====================

struct MaterialData
{
	// Global Var
    float2 u_textureTiling;
    float2 u_textureOffset;
    float4 u_diffuse;
    float u_shininess;
    float3 u_specular;
    float u_heightScale;
    float3 padding;
};

// Material Buffer Name Must Be "Material"
cbuffer Material : register(b10)
{
    MaterialData materialData;
}; 

struct Pixel
{
    float4 position : SV_POSITION;
    float3 fragPos: POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

Texture2D u_normalMap : register(t101);
Texture2D u_diffuseMap : register(t102);
Texture2D u_specularMap : register(t103);
Texture2D u_heightMap : register(t104);
Texture2D u_maskMap : register(t105);

Pixel mainVS(Vertex_PosUvNorTan input)
{
    Pixel output;

    input.position.w = 1.0f;
    output.fragPos = mul(input.position, buffer_pass.transform).xyz;
    output.position = mul(input.position, buffer_pass.transform);
    output.position = mul(output.position, buffer_frame.view_projection_unjittered);
    output.normal = mul(float4(input.normal, 0), buffer_pass.transform).xyz;
    output.tangent = mul(float4(input.tangent, 0), buffer_pass.transform).xyz;
    output.uv = input.uv;

    return output;
}

/**
 * \brief Light Render Model For BilinnPhong
 * \param viewDir 
 * \param normal 
 * \param diffuseTex 
 * \param specularTex 
 * \param shininess 
 * \param lightDir 
 * \param lightColor 
 * \param luminosity 
 * \return 
 */
float3 BilinnPhong(float3 viewDir, float3 normal, float3 diffuseTex, float3 specularTex, float shininess, float3 lightDir, float3 lightColor, float luminosity)
{
    const float3 halfwayDir = normalize(lightColor + viewDir);
    const float diffuseCoefficient = max(dot(normal, lightDir), 0.0);
    const float specularCoefficient = pow(max(dot(normal, halfwayDir), 0.0), shininess * 2.0);

    return lightColor * diffuseTex.rgb * diffuseCoefficient * luminosity + ((luminosity > 0.0) ?
    (lightColor * specularTex.rgb * specularCoefficient * luminosity) : float3(0, 0, 0));
}

float3 CalcPointLight(float3 viewDir, float3 normal, float3 diffuseTex, float3 specularTex, float shininess, LightBufferData light_buffer_data)
{
    float3 lightPosition = light_buffer_data.position.xyz;
    float3 lightDir = light_buffer_data.direction.xyz;
    float3 lightColor = light_buffer_data.color.rgb;
    float luminosity = light_buffer_data.intensity_range_angle_bias[0];


    return BilinnPhong(viewDir, normal, diffuseTex, specularTex, shininess, lightDir, lightColor, luminosity);
}


float3 CalcDirectionalLight(float3 viewDir, float3 normal, float3 diffuseTex, float3 specularTex, float shininess, LightBufferData light_buffer_data, float luminosity)
{
    return BilinnPhong(viewDir, normal, diffuseTex, specularTex, shininess, light_buffer_data.direction.xyz, light_buffer_data.color.rgb, luminosity);
}


// return shadow ratio
float ShadowCalculation(float3 fragWorldPos)
{
    // todo 只使用0

	// project into light space
    int shadowIndex = 0;
    int subIndex = 0;
    float3 pos_ndc = world_to_ndc(fragWorldPos, light_buffer_data_arr.lightBufferDataArr[shadowIndex].view_projection[subIndex]);
	float2 pos_uv = ndc_to_uv(pos_ndc);
    // float2 pos_uv = pos_ndc * 0.5 + 0.5;

    //// 执行透视除法
    //float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //// 变换到[0,1]的范围
    //projCoords = projCoords * 0.5 + 0.5;

    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = tex_light_directional_depth.SampleLevel(samplers[sampler_point_clamp], float3(pos_uv, subIndex), 0).r;
    // 取得当前片段在光源视角下的深度
    //float currentDepth = projCoords.z;
    float currentDepth = pos_ndc.z;
    // 检查当前片段是否在阴影中
    float bias = 0.00;
    float shadow = currentDepth - bias < closestDepth ? 1.0 : 0.0;

    return shadow;
}


float4 mainPS(Pixel input) : SV_Target
{
    float2 g_TexCoords = materialData.u_textureOffset + float2((input.uv.x * materialData.u_textureTiling.x) % 1.0, (input.uv.y * materialData.u_textureTiling.y) % 1.0);

    float shadow = ShadowCalculation(input.fragPos);

    float3 viewDir = normalize(buffer_frame.camera_position - input.fragPos);
    float4 diffuseTexel = u_diffuseMap.Sample(samplers[sampler_point_wrap], g_TexCoords) * materialData.u_diffuse;
    float4 specularTexel = u_specularMap.Sample(samplers[sampler_point_wrap], g_TexCoords) * float4(materialData.u_specular, 1.0);
    float3 normal = normalize(input.normal);

    int lightCount = light_buffer_data_arr.lightCount;
    // int lightCount = 2;
    float3 lightSum = float3(0, 0, 0);
    for (int index = 0; index < lightCount; index++)
    {
        lightSum += BilinnPhong(viewDir, normal, diffuseTexel.rgb, specularTexel.rgb,
				materialData.u_shininess, -light_buffer_data_arr.lightBufferDataArr[index].direction.xyz, light_buffer_data_arr.lightBufferDataArr[index].color.xyz, light_buffer_data_arr.lightBufferDataArr[index].intensity_range_angle_bias[0]);
    }

    // float4 color = float4(lightSum, light_buffer_data_arr.lightCount);
    // float4 color = diffuseTexel;

    float3 shadowedColor = (1.0f - shadow) * lightSum;
    float4 color = float4(shadowedColor, diffuseTexel.a);
    return color;

    //float3 pos_ndc = world_to_ndc(input.fragPos, light_buffer_data_arr.lightBufferDataArr[0].view_projection[0]);
    //float2 pos_uv = ndc_to_uv(pos_ndc);
    //float closestDepth = tex_light_directional_depth.SampleLevel(samplers[sampler_point_clamp], float3(pos_uv, 0), 0).r;
    //// return float4(pos_uv, 1.0f, diffuseTexel.a);
    //return float4(input.fragPos, diffuseTexel.a);

}
