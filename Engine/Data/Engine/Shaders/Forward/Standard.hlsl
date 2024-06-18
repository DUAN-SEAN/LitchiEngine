
// = INCLUDES ========
#include "../Common/common_light.hlsl"
//====================

// texture must > 100
// material must equal b10

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
    float3 fragPos : POSITION;
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
    output.position = mul(output.position, buffer_rendererPath.view_projection_unjittered);
    output.normal = mul(float4(input.normal, 0), buffer_pass.transform).xyz;
    output.tangent = mul(float4(input.tangent, 0), buffer_pass.transform).xyz;
    output.uv = input.uv;

    return output;
}


float4 mainPS(Pixel input) : SV_Target
{
    float2 g_TexCoords = materialData.u_textureOffset + float2((input.uv.x * materialData.u_textureTiling.x) % 1.0, (input.uv.y * materialData.u_textureTiling.y) % 1.0);

    float3 viewDir = normalize(buffer_rendererPath.camera_position - input.fragPos);
    float4 diffuseTexel = u_diffuseMap.Sample(samplers[sampler_point_wrap], g_TexCoords) * materialData.u_diffuse;
    float4 specularTexel = u_specularMap.Sample(samplers[sampler_point_wrap], g_TexCoords) * float4(materialData.u_specular, 1.0);
    float3 normal = normalize(input.normal);
    
    uint lightCount = (uint) pass_get_f3_value2().z;
    
    float3 shadowedColor = float3(0, 0, 0);
    for (int index = 0; index < lightCount; index++)
    {
        LightBufferData lightData = buffer_lights[index];
        float attenuation = compute_attenuation(lightData, input.fragPos);
        float3 lightSum = BilinnPhong(viewDir, normal, diffuseTexel.rgb, specularTexel.rgb,
				materialData.u_shininess, -lightData.direction.xyz, lightData.color.xyz,
        lightData.intensity * attenuation);

        // temp code
        float shadow = 0;
        bool t = pass_is_transparent();
        if (t)
        {
            shadow = ShadowCalculation2(normal, input.fragPos, index);
        }
        else
        {
            shadow = ShadowCalculation2(normal, input.fragPos, index);
        }
        shadowedColor += (shadow) * lightSum;
    }

    float4 color = float4(shadowedColor, diffuseTexel.a);
    return color;
}
