// = INCLUDES ========
#include "../../Common/common_light.hlsl"
#include "../../Common/common_colorspace.hlsl"
//====================

struct MaterialData
{
	// Global Var
	float2 u_textureTiling;
    float2 u_textureOffset;
    float4 u_color;
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

Texture2D u_albedo : register(t101);
Texture2D u_normal : register(t102);
Texture2D u_metallic : register(t103);
Texture2D u_roughness : register(t104);
Texture2D u_aO : register(t105);

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
	// sample for texture
    float2 g_TexCoords = materialData.u_textureOffset + float2((input.uv.x * materialData.u_textureTiling.x) % 1.0, (input.uv.y * materialData.u_textureTiling.y) % 1.0);
    float4 albedo = u_albedo.Sample(samplers[sampler_point_wrap], g_TexCoords);
    float3 albedoLinear = srgb_to_linear(albedo.xyz);
    albedo = float4(albedoLinear * materialData.u_color.rgb, albedo.a * materialData.u_color.a);
    if (!pass_is_transparent())
    {
        albedo.a = 1.0f;
    }
	
    float metallic = u_metallic.Sample(samplers[sampler_point_wrap], g_TexCoords).x;
    float perceptualRoughness = 1 - u_roughness.Sample(samplers[sampler_point_wrap], g_TexCoords).x;
    float roughness = perceptualRoughness * perceptualRoughness;
    float squareRoughness = roughness * roughness;
    float lerpSquareRoughness = pow(lerp(0.002, 1, roughness), 2);

	// get normal by sample
    float3 sampleNormal = u_normal.Sample(samplers[sampler_point_wrap], g_TexCoords).xyz;
    float3 binormal = cross(normalize(input.normal), normalize(input.tangent));
    float3x3 rotation = float3x3(input.tangent, binormal, sampleNormal);
    float3 normal = mul(rotation, sampleNormal);
    normal = normalize(input.normal);

    float3 viewDir = normalize(buffer_rendererPath.camera_position.xyz - input.fragPos.xyz);
    float nv = max(saturate(dot(normal, viewDir)), 0.000001);

	// calculate for each light source
	uint index_light = (uint)pass_get_f3_value2().y;
	uint index_array = (uint)pass_get_f3_value2().x;
	uint lightCount = (uint)pass_get_f3_value2().z;
	float3 lightSum = float3(0, 0, 0);
	for (int index = 0; index < lightCount; index++) {
		
        LightBufferData lightBufferData = buffer_lights[index];
        
        lightSum += CalcOneLightColorPBR(input.fragPos, albedo, metallic, squareRoughness, lerpSquareRoughness,
		                  normal, viewDir, nv, index);
    }
	
    // float3 color = lightSum + float3(0.03) * albedo.xyz * ao;
    float3 color = linear_to_srgb(lightSum + float3(0.03, 0.03, 0.03) * albedo.xyz);
	
    return float4(color.x, color.y, color.z, albedo.a);
}