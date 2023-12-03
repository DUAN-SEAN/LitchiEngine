
// = INCLUDES ========
#include "Common/common.hlsl"
//====================

struct PixelOutputType
{
    float4 color             : SV_Target0;
    // float fsr2_reactive_mask : SV_Target1;
};

Pixel_PosColor mainVS(Vertex_PosColor input)
{
    Pixel_PosColor output;

    input.position.w = 1.0f;
    output.position  = mul(input.position, buffer_pass.transform);
    output.position  = mul(output.position, buffer_frame.view_projection_unjittered);
    output.color     = input.color;
    
    return output;
}

PixelOutputType mainPS(Pixel_PosColor input)
{
    PixelOutputType output;

    output.color              = input.color;
    // output.fsr2_reactive_mask = input.color.a;

    return output;
}
