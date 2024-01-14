
//= INCLUDES =========
#include "../Common/common.hlsl"
//====================

Pixel_PosUv mainVS(Vertex_PosUv input)
{
    Pixel_PosUv output;

    input.position.w = 1.0f;
    output.position = mul(input.position, buffer_pass.transform);
    output.position = mul(output.position, buffer_frame.view_projection_orthographic);
    output.uv = input.uv;

    return output;
}

float4 mainPS(Pixel_PosUv input) : SV_TARGET
{
    // Sample text from texture atlas
    float4 color = tex.Sample(samplers[sampler_bilinear_clamp], input.uv);

    // Color it
    color *= float4(pass_get_f4_value().rgb, 1.0f);

    return color;
}

