
//= INCLUDES =========
#include "Common/common.hlsl"
//====================

Pixel_PosUv mainVS(Vertex_PosUvNorTan input)
{
    Pixel_PosUv output;

    input.position.w     = 1.0f; 
    output.position      = mul(input.position, buffer_pass.transform);
    // output.position      = mul(output.position, buffer_light.view_projection[0]);

    output.uv = input.uv;

    return output;
}

// transparent/colored shadows
float4 mainPS(Pixel_PosUv input) : SV_TARGET
{
    return input.position;
}
