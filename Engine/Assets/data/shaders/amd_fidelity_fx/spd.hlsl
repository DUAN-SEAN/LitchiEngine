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

//= INCLUDES ============
#include "../common.hlsl"
//=======================

#define A_GPU
#define A_HLSL
#define SPD_NO_WAVE_OPERATIONS
#define SPD_LINEAR_SAMPLER

#include "ffx_a.h"

groupshared AF4 spd_intermediate[16][16];
groupshared AU1 spd_counter;

AF4 SpdLoadSourceImage(ASU2 p, AU1 slice)
{
    float2 uv = (p + 0.5f) / pass_get_resolution_out();
    return tex.SampleLevel(samplers[sampler_bilinear_clamp], uv, 0);
}

// Load from mip 5
AF4 SpdLoad(ASU2 pos, AU1 slice)
{
    return tex_uav_mips[5][pos];
}

void SpdStore(ASU2 pos, AF4 value, AU1 index, AU1 slice)
{
    tex_uav_mips[index][pos] = value;
}

AF4 SpdLoadIntermediate(AU1 x, AU1 y)
{
    return spd_intermediate[x][y];
}

void SpdStoreIntermediate(AU1 x, AU1 y, AF4 value)
{
    spd_intermediate[x][y] = value;
}

AF4 SpdReduce4(AF4 s1, AF4 s2, AF4 s3, AF4 s4)
{
    // luminance weighted average
    float s1w = 1 / (luminance(s1) + 1);
    float s2w = 1 / (luminance(s2) + 1);
    float s3w = 1 / (luminance(s3) + 1);
    float s4w = 1 / (luminance(s4) + 1);
    float one_div_wsum = 1.0 / (s1w + s2w + s3w + s4w);
    return (s1 * s1w + s2 * s2w + s3 * s3w + s4 * s4w) * one_div_wsum;
}

void SpdIncreaseAtomicCounter(AU1 slice)
{
    InterlockedAdd(g_atomic_counter[0], 1, spd_counter);
}

AU1 SpdGetAtomicCounter()
{
    return spd_counter;
}

void SpdResetAtomicCounter(AU1 slice)
{
    g_atomic_counter[0] = 0;
}

#include "ffx_spd.h"

[numthreads(256, 1, 1)]
void mainCS(uint3 work_group_id : SV_GroupID, uint local_thread_index : SV_GroupIndex)
{
    const float3 f3_value  = pass_get_f3_value();
    float mip_count        = f3_value.x;
    float work_group_count = f3_value.y;
    SpdDownsample(work_group_id.xy, local_thread_index, mip_count, work_group_count, work_group_id.z);
}
