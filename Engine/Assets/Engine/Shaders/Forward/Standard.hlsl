
// = INCLUDES ========
#include "../Common/common.hlsl"
//====================

// Global Var
float2 u_TextureOffset;
float2 u_TextureTiling;

Texture2D u_shadowMap : register(t100);

Pixel_PosUvNorTan mainVS(Vertex_PosUvNorTan input)
{
    Pixel_PosUvNorTan output;

    input.position.w = 1.0f;
    output.position = mul(input.position, buffer_pass.transform);
    output.position = mul(output.position, buffer_frame.view_projection_unjittered);
    output.normal = mul(input.normal, buffer_pass.transform);
    output.tangent = mul(input.tangent, buffer_pass.transform);
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

// return shadow ratio
float ShadowCalculation(float4 fragPosLightSpace)
{
    // 执行透视除法
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = u_shadowMap.Sample(samplers[sampler_point_wrap], projCoords.x).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}


float4 mainPS(Pixel_PosUvNorTan input) : SV_Target
{
    float2 g_TexCoords = u_TextureOffset + float2((input.uv.x * u_TextureTiling.x) % 1.0, (input.uv.y * u_TextureTiling.y) % 1.0);

    // float shadow = ShadowCalculation(input.ShadowCoord);



    // just a color
    return pass_get_f4_value();
}
