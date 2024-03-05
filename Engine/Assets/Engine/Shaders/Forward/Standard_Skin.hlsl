
// = INCLUDES ========
#include "../Common/common_light.hlsl"
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
    float3 fragPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    int4 indexs : POSITION2;
};

Texture2D u_normalMap : register(t101);
Texture2D u_diffuseMap : register(t102);
Texture2D u_specularMap : register(t103);
Texture2D u_heightMap : register(t104);
Texture2D u_maskMap : register(t105);

Pixel mainVS(Vertex_PosUvNorTanBone input)
{
    Pixel output;

    float4 weights = float4(input.boneWeights.x, input.boneWeights.y, input.boneWeights.z,
    1.0f - input.boneWeights.x - input.boneWeights.y - input.boneWeights.z);
    matrix boneTransform = bone_data_arr.boneTransformArr[(int) input.boneIndices[0]] * weights[0];
    boneTransform += bone_data_arr.boneTransformArr[(int) input.boneIndices[1]] * weights[1];
    boneTransform += bone_data_arr.boneTransformArr[(int) input.boneIndices[2]] * weights[2];
    boneTransform += bone_data_arr.boneTransformArr[(int) input.boneIndices[3]] * weights[3];

    output.indexs[0] = (int) input.boneIndices[0];
    output.indexs[1] = (int) input.boneIndices[1];
    output.indexs[2] = (int) input.boneIndices[2];
    output.indexs[3] = (int) input.boneIndices[3];

    input.position.w = 1.0f;
    float4 posL = mul(input.position, boneTransform);
    float4 normalL = mul(float4(input.normal, 0), boneTransform);
    float4 tangentL = mul(float4(input.tangent, 0), boneTransform);

    output.fragPos = mul(posL, buffer_pass.transform).xyz;
    output.position = mul(posL, buffer_pass.transform);
    output.position = mul(output.position, buffer_rendererPath.view_projection_unjittered);
    output.normal = mul(normalL, buffer_pass.transform).xyz;
    output.tangent = mul(tangentL, buffer_pass.transform).xyz;
    output.uv = input.uv;

    return output;
}


float4 mainPS(Pixel input) : SV_Target
{
    float2 g_TexCoords = materialData.u_textureOffset + float2((input.uv.x * materialData.u_textureTiling.x) % 1.0, (input.uv.y * materialData.u_textureTiling.y) % 1.0);

    float shadow = ShadowCalculation(input.fragPos);

    float3 viewDir = normalize(buffer_rendererPath.camera_position - input.fragPos);
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

    //float3 shadowedColor = (1.0f - shadow) * lightSum;
    float3 shadowedColor = shadow * lightSum;
    float4 color = float4(shadowedColor, diffuseTexel.a);
    return color;
    //return float4(shadow, shadow, shadow,1.0f);

    //float3 pos_ndc = world_to_ndc(input.fragPos, light_buffer_data_arr.lightBufferDataArr[0].view_projection[0]);
    //float2 pos_uv = ndc_to_uv(pos_ndc);
    //float closestDepth = tex_light_directional_depth.SampleLevel(samplers[sampler_point_clamp], float3(pos_uv, 0), 0).r;
    //// return float4(pos_uv, 1.0f, diffuseTexel.a);
    //return float4(input.fragPos, diffuseTexel.a);

}
