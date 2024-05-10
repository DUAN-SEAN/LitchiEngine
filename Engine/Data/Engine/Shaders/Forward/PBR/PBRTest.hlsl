// = INCLUDES ========
#include "../../Common/common_light.hlsl"
//====================

struct MaterialData
{
	// Global Var
	float2 u_textureTiling;
	float2 u_textureOffset;
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

Texture2D u_albedo : register(t101);
Texture2D u_normal : register(t102);
Texture2D u_metallic : register(t103);
Texture2D u_roughness : register(t104);
Texture2D u_aO : register(t105);

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

static const float gamma_sdr = 2.2f; // SDR monitors are likely old, and aim for a simplistic gamma 2.2 curve
static const float gamma_hdr = 2.4f; // HDR monitors are more likely to aim for the actual sRGB standard, which has a curve that for mid tones to high lights resembles a gamma of 2.4

float3 srgb_to_linear(float3 color)
{
    float gamma = lerp(gamma_sdr, gamma_hdr, 0.0f);
    float3 linear_low = color / 12.92;
    float3 linear_high = pow((color + 0.055) / 1.055, gamma);
    float3 is_high = step(0.0404482362771082, color);
    return lerp(linear_low, linear_high, is_high);
}


float4 mainPS(Pixel input) : SV_Target
{
	float PI = 3.14;
	float3 colorSpaceDielectricSpecRgb = float3(0.04, 0.04, 0.04);
	//input.normal = normalize(input.normal);
	
	// sample for texture
	float2 g_TexCoords = materialData.u_textureOffset + float2((input.uv.x * materialData.u_textureTiling.x) % 1.0, (input.uv.y * materialData.u_textureTiling.y) % 1.0);
	float4 albedo = u_albedo.Sample(samplers[sampler_point_wrap], g_TexCoords);
    float3 albedoLinear = srgb_to_linear(albedo.xyz);
    albedo = float4(albedoLinear, albedo.a);
	
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

	float3 viewDir = normalize(buffer_rendererPath.camera_position.xyz - input.fragPos.xyz);
	float nv = max(saturate(dot(normal, viewDir)), 0.000001);

	// calculate for each light source
	uint index_light = (uint)pass_get_f3_value2().y;
	uint index_array = (uint)pass_get_f3_value2().x;
	uint lightCount = (uint)pass_get_f3_value2().z;
	float3 lightSum = float3(0, 0, 0);
	for (int index = 0; index < lightCount; index++) {
		LightBufferData lightBufferData = buffer_lights[index];

		float3 lightDir = normalize(-lightBufferData.direction.xyz).xyz;
        // float3 lightDir = normalize(lightBufferData.position - input.fragPos.xyz);
        float3 lightColor = lightBufferData.color.xyz * lightBufferData.intensity;
		float3 halfVector = normalize(lightDir + viewDir);

		float nl = max(saturate(dot(normal, lightDir)), 0.000001);
		float lh = max(saturate(dot(lightDir, halfVector)), 0.000001);
		float vh = max(saturate(dot(viewDir, halfVector)), 0.000001);
		float nh = max(saturate(dot(normal, halfVector)), 0.000001);

		// calculate D F G for Specular
		float D = lerpSquareRoughness / (pow((pow(nh, 2) * (lerpSquareRoughness - 1) + 1), 2) * PI);

		float3 F0 = lerp(colorSpaceDielectricSpecRgb, albedo.xyz, metallic);
		float3 F = F0 + (1 - F0) * exp2((-5.55473 * vh - 6.98316) * vh);

		float kInDirectLight = pow(squareRoughness + 1, 2) / 8;
		float kInIBL = pow(squareRoughness, 2) / 8;
		float GLeft = nl / lerp(nl, 1, kInDirectLight);
		float GRight = nv / lerp(nv, 1, kInDirectLight);
		float G = GLeft * GRight;

		float3 kd = (1 - F) * (1 - metallic);

		// calculate directLightResult with diffuse and specular
        float3 diffColor = kd * albedo.xyz / PI;
        float3 specColor = (D * G * F) / (4 * nv * nl);
        float3 directLightResult = (diffColor + specColor) * lightColor * nl;

		// calculate indirectLightResult todo
		float3 iblDiffuseResult = 0;
		float3 iblSpecularResult = 0;
		float3 indirectResult = iblDiffuseResult + iblSpecularResult;

		lightSum += directLightResult + indirectResult;
	}

    // float3 color = lightSum + float3(0.03) * albedo.xyz * ao;
    float3 color = lightSum + float3(0.03,0.03,0.03) * albedo.xyz;
	
    return float4(color.x, color.y, color.z, 1);
}