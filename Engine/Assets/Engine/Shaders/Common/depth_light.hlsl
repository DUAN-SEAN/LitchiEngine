
//= INCLUDES =========
#include "common.hlsl"
//====================

Pixel_PosUv mainVS(Vertex_PosUvNorTan input)
{
    Pixel_PosUv output;

    input.position.w     = 1.0f; 
    output.position      = mul(input.position, buffer_pass.transform);
    #if INSTANCED
    output.position      = mul(output.position, input.instance_transform);
    #endif
    output.position      = mul(output.position, buffer_light.view_projection[0]);

    output.uv = input.uv;

    return output;
}

// transparent/colored shadows
float4 mainPS(Pixel_PosUv input) : SV_TARGET
{
    float2 uv    = float2(input.uv.x * buffer_material.tiling.x + buffer_material.offset.x, input.uv.y * buffer_material.offset.y + buffer_material.tiling.y);
    float4 color = tex.SampleLevel(samplers[sampler_anisotropic_wrap], uv, 0);
    return float4(degamma(color.rgb), color.a) * buffer_material.color;
}
