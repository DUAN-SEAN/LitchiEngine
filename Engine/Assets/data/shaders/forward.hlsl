
//= INCLUDES =========
#include "common.hlsl"
//====================

Pixel_PosUvNorTan mainVS(Vertex_PosUvNorTan input)
{
    Pixel_PosUvNorTan output;

    output.position = mul(buffer_pass.transform, input.position);
    output.uv = input.uv;
    output.normal = normalize(mul(input.normal, (float3x3) buffer_pass.transform)).xyz;
    output.tangent = normalize(mul(input.tangent, (float3x3) buffer_pass.transform)).xyz;

    return output;
}

float4 mainPS(Pixel_PosUvNorTan input) : SV_Target
{
    float4 color_texture = tex.Sample(samplers[sampler_point_wrap], input.uv);
    return color_texture;
 }
