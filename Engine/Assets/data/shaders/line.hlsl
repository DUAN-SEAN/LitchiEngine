/*
Copyright(c) 2016-2023 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// = INCLUDES ========
#include "common.hlsl"
//====================

struct PixelOutputType
{
    float4 color             : SV_Target0;
    float fsr2_reactive_mask : SV_Target1;
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
    output.fsr2_reactive_mask = input.color.a;

    return output;
}
