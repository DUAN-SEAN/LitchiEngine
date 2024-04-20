
//= INCLUDES =========
#include "Common/common.hlsl"
//====================

struct vertex
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

vertex mainVS(vertex input)
{
    input.position.w = 1.0f;
    input.position  = mul(input.position, buffer_pass.transform);

    return input;
}

float4 mainPS(vertex input) : SV_TARGET
{
    float4 color = tex.Sample(samplers[sampler_bilinear_clamp], input.uv);
    // color.rgb    = srgb_to_linear(color.rgb);
    
    return color;
}
