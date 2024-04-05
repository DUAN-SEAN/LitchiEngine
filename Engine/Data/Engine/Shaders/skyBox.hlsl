//= INCLUDES =========
#include "Common/common.hlsl"
//====================

struct Pixel_PosUvwNorTan
{
    float4 position : SV_POSITION;
    float3 uvw : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

Pixel_PosUvwNorTan mainVS(Vertex_PosUvNorTan input)
{
    Pixel_PosUvwNorTan output;
    output.position = mul(input.position, buffer_pass.transform);
    output.position = mul(output.position, buffer_rendererPath.view_projection);
    output.uvw = input.position.xyz;

    return output;
}

float4 mainPS(Pixel_PosUvwNorTan input) : SV_Target
{
    float4 color_texture = tex_skyBox.Sample(samplers[sampler_point_wrap], input.uvw);
    return color_texture;
}
