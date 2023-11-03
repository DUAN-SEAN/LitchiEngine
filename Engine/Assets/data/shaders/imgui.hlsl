
//= INCLUDES =========
#include "common.hlsl"
//====================

Pixel_PosColUv mainVS(Vertex_Pos2dUvColor input)
{
    Pixel_PosColUv output;

    output.position = mul(buffer_pass.transform, float4(input.position.x, input.position.y, 0.0f, 1.0f));
    output.color    = input.color;
    output.uv       = input.uv;

    return output;
}

float4 mainPS(Pixel_PosColUv input) : SV_Target
{
     // texture visualization options
    float4 channels     = pass_get_f4_value();
    float3 f3_value     = pass_get_f3_value();
    bool gamma_correct  = f3_value.x == 1.0f;
    bool packed         = f3_value.y == 1.0f;
    bool boost          = f3_value.z == 1.0f;
    float3 f3_value2    = pass_get_f3_value2();
    bool absolute       = f3_value2.x == 1.0f;
    bool point_sampling = f3_value2.y == 1.0f;
    float mip           = f3_value2.z;
    bool is_visualized  = pass_is_transparent();
   
    float4 color_texture = point_sampling ? tex.SampleLevel(samplers[sampler_point_wrap], input.uv, mip) : tex.SampleLevel(samplers[sampler_bilinear_wrap], input.uv, mip);
 
    if (is_visualized)
    {
        color_texture.r *= channels.r ? 1.0f : 0.0f;
        color_texture.g *= channels.g ? 1.0f : 0.0f;
        color_texture.b *= channels.b ? 1.0f : 0.0f;
        color_texture.a  = channels.a ? color_texture.a : 1.0f;
    
        if (gamma_correct)
        {
            color_texture.rgb = gamma(color_texture.rgb);
        }
    
        if (absolute)
        {
            color_texture = abs(color_texture);
        }
    
        if (packed)
        {
            color_texture.rgb = pack(color_texture.rgb);
        }
    
        if (boost)
        {
            color_texture.rgb *= 10.0f;
        }
    }

    float4 color_vertex = input.color;
    return color_vertex * color_texture;
    // return color_vertex;
 }
