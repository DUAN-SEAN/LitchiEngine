
//= INCLUDES =========
#include "Common/common.hlsl"
//====================

Pixel_PosUvNorTan mainVS(Vertex_PosUvNorTan input)
{
    Pixel_PosUvNorTan output;

    output.position = mul(input.position,buffer_pass.transform);
    output.position = mul(output.position, buffer_rendererPath.view_projection);
    output.uv = input.uv;
    output.normal = normalize(mul(input.normal, (float3x3) buffer_pass.transform)).xyz;
    output.normal = normalize(mul(output.normal, (float3x3) buffer_rendererPath.view_projection)).xyz;
    output.tangent = normalize(mul(input.tangent, (float3x3) buffer_pass.transform)).xyz;
    output.tangent = normalize(mul(output.tangent, (float3x3) buffer_rendererPath.view_projection)).xyz;

    return output;
}

float4 mainPS(Pixel_PosUvNorTan input) : SV_Target
{
    //float4 color_texture = tex.Sample(samplers[sampler_point_wrap], input.uv);
    //return color_texture;
    return float4(0.644f, 0.003f, 0.005f,1.0f);

}
