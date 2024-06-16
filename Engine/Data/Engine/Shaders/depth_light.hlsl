
//= INCLUDES =========
#include "Common/common.hlsl"
//====================

static const float ALPHA_THRESHOLD_DEFAULT = 0.6f;

Pixel_PosUv mainVS(Vertex_PosUvNorTan input)
{
    Pixel_PosUv output;
    
    output.uv = input.uv;

    //input.position.w     = 1.0f; 
    //output.position      = mul(input.position, buffer_pass.transform);
    
    float3 f3_value2 = pass_get_f3_value2();
    uint index_array = (uint) f3_value2.x;
    uint index_light = (uint) f3_value2.y;
    uint light_count = (uint) f3_value2.z;
    LightBufferData lightBufferData = buffer_lights[index_light];
    input.position.w     = 1.0f; 
    output.position = mul(input.position, buffer_pass.transform);
    output.position = mul(output.position,lightBufferData.view_projection[index_array]);

    // for point lights, output.position is in view space this because we do the paraboloid projection here
    if (lightBufferData.light_is_point())
    {
        float3 ndc = project_onto_paraboloid(output.position.xyz, 0.01f, lightBufferData.angle);
        output.position = float4(ndc, 1.0);
    }

    return output;
}

// transparent/colored shadows
float4 mainPS(Pixel_PosUv input) : SV_TARGET
{
    // alpha test
    const float3 f3_value = pass_get_f3_value();
    const bool has_alpha_mask = f3_value.x == 1.0f;
    const bool has_albedo = f3_value.y == 1.0f;
    //float alpha_mask = has_alpha_mask ? GET_TEXTURE(material_mask).Sample(samplers[sampler_point_wrap], input.uv).r : 1.0f;
    float alpha_mask = 1.0f;
    //bool alpha_albedo = has_albedo ? GET_TEXTURE(material_albedo).Sample(samplers[sampler_point_wrap], input.uv).a : 1.0f;
    bool alpha_albedo = 1.0f;
    if (min(alpha_mask, alpha_albedo) <= ALPHA_THRESHOLD_DEFAULT)
        discard;

    // colored transparent shadows
    //return GetMaterial().color;
    
    return input.position;
}
