
#include "common_textures.hlsl"
#include "common_samplers.hlsl"

struct FrameBufferData
{
    //matrix view;
    //matrix projection;
    //matrix view_projection;
    //matrix view_projection_inverted;
    //matrix view_projection_orthographic;
    //matrix view_projection_unjittered;
    //matrix view_projection_previous;

    float2 resolution_render;
    float2 resolution_output;

    float2 taa_jitter_current;
    float2 taa_jitter_previous;
    
    float delta_time;
    uint frame;
    float gamma;
    uint options;
    
    //float3 camera_position;
    //float camera_near;
    
    //float3 camera_direction;
    //float camera_far;
};

struct RendererPathBufferData
{
    matrix view;
    matrix projection;
    matrix view_projection;
    matrix view_projection_inverted;
    matrix view_projection_orthographic;
    matrix view_projection_unjittered;
    matrix view_projection_previous;
    
    float3 camera_position;
    float camera_near;
    
    float3 camera_direction;
    float camera_far;
};

struct LightBufferData
{
    matrix view_projection[6];
    
    float4 color;

    float3 position;
    float intensity;

    float3 direction;
    float range;
    
    float angle;
    uint flags;
    float2 padding;

    
    // lighting properties
    bool light_is_directional()
    {
        return flags & uint(1U << 0);
    }
    bool light_is_point()
    {
        return flags & uint(1U << 1);
    }
    bool light_is_spot()
    {
        return flags & uint(1U << 2);
    }
    bool light_has_shadows()
    {
        return flags & uint(1U << 3);
    }
    bool light_has_shadows_transparent()
    {
        return flags & uint(1U << 4);
    }
    bool light_is_volumetric()
    {
        return flags & uint(1U << 5);
    }
    
    float compare_depth(float3 uv, float compare)
    {
        return tex_light_directional_depth.SampleCmpLevelZero(samplers_comparison[sampler_compare_depth], uv, compare).r;
        //// float3 -> uv, slice
        //if (light_is_directional())
        //    return tex_light_directional_depth.SampleCmpLevelZero(samplers_comparison[sampler_compare_depth], uv, compare).r;
        
        //// float3 -> direction
        //if (light_is_point())
        //    return tex_light_point_depth.SampleCmpLevelZero(samplers_comparison[sampler_compare_depth], uv, compare).r;
        
        //// float3 -> uv, 0
        //if (light_is_spot()) 
        //    return tex_light_spot_depth.SampleCmpLevelZero(samplers_comparison[sampler_compare_depth], uv.xy, compare).r;
    
        //return 0.0f;
    }
    
    float sample_depth(float3 uv)
    {
        return tex_light_directional_depth.SampleLevel(samplers[sampler_bilinear_clamp_border], uv, 0).r;
        //// float3 -> uv, slice
        //if (light_is_directional())
        //    return tex_light_directional_depth.SampleLevel(samplers[sampler_bilinear_clamp_border], uv, 0).r;
        
        //// float3 -> direction
        //if (light_is_point())
        //    return tex_light_point_depth.SampleLevel(samplers[sampler_bilinear_clamp_border], uv, 0).r;
    
        //// float3 -> uv, 0
        //if (light_is_spot())
        //    return tex_light_spot_depth.SampleLevel(samplers[sampler_bilinear_clamp_border], uv.xy, 0).r;
    
        //return 0.0f;
    }
    
    float3 sample_color(float3 uv)
    {
        return tex_light_directional_color.SampleLevel(samplers[sampler_bilinear_clamp_border], uv, 0).rgb;
        //// float3 -> uv, slice
        //if (light_is_directional())
        //    return tex_light_directional_color.SampleLevel(samplers[sampler_bilinear_clamp_border], uv, 0).rgb;
    
        //// float3 -> direction
        //if (light_is_point())
        //    return tex_light_point_color.SampleLevel(samplers[sampler_bilinear_clamp_border], uv, 0).rgb;
    
        //// float3 -> uv, 0
        //if (light_is_spot())
        //    return tex_light_spot_color.SampleLevel(samplers[sampler_bilinear_clamp_border], uv.xy, 0).rgb;
        
        //return 0.0f;
    }
    
    float2 compute_resolution()
    {
        float2 resolution;
        
        //if (light_is_directional())
        //{
        //    uint layer_count;
        //    tex_light_directional_depth.GetDimensions(resolution.x, resolution.y, layer_count);
        //}
        //else if (light_is_point())
        //{
        //    tex_light_point_depth.GetDimensions(resolution.x, resolution.y);
        //}
        //else if (light_is_spot())
        //{
        //    tex_light_spot_depth.GetDimensions(resolution.x, resolution.y);
        //}
        
        uint layer_count;
        tex_light_directional_depth.GetDimensions(resolution.x, resolution.y, layer_count);

        return resolution;
    }


};
RWStructuredBuffer<LightBufferData> buffer_lights : register(u1);

struct MaterialBufferData
{
    float4 color;

    float2 tiling;
    float2 offset;

    float roughness;
    float metallness;
    float normal;
    float height;

    uint properties;
    float clearcoat;
    float clearcoat_roughness;
    float anisotropic;
    
    float anisotropic_rotation;
    float sheen;
    float sheen_tint;
    float padding3;
};

const static int MaxLight = 15;
struct LightBufferDataArr
{
    int lightCount;
    int3 padding;
    LightBufferData lightBufferDataArr[MaxLight];
};

const static int MaxBone = 250;
struct BoneDataArr
{
    //int boneCount;
    //int3 padding;
    matrix boneTransformArr[MaxBone];
};

cbuffer BufferFrame : register(b0)
{
    FrameBufferData buffer_frame;
}; // low frequency    - updates once per frame
cbuffer BufferLight : register(b1)
{
    LightBufferData buffer_light;
}; // medium frequency - updates per light
cbuffer BufferMaterial : register(b2)
{
    MaterialBufferData buffer_material;
}; // medium frequency - updates per material during the g-buffer pass
//cbuffer BufferLightArr : register(b3)
//{
//    LightBufferDataArr light_buffer_data_arr;
//}
cbuffer BoneDataArr : register(b4)
{
    BoneDataArr bone_data_arr;
}
cbuffer BufferRendererPath : register(b5)
{
    RendererPathBufferData buffer_rendererPath;
}

struct PassBufferData
{
    matrix transform;
    matrix m_value;
};

[[vk::push_constant]]
PassBufferData buffer_pass;

// g-buffer texture properties
bool has_single_texture_roughness_metalness()
{
    return buffer_material.properties & uint(1U << 0);
}
bool has_texture_height()
{
    return buffer_material.properties & uint(1U << 1);
}
bool has_texture_normal()
{
    return buffer_material.properties & uint(1U << 2);
}
bool has_texture_albedo()
{
    return buffer_material.properties & uint(1U << 3);
}
bool has_texture_roughness()
{
    return buffer_material.properties & uint(1U << 4);
}
bool has_texture_metalness()
{
    return buffer_material.properties & uint(1U << 5);
}
bool has_texture_alpha_mask()
{
    return buffer_material.properties & uint(1U << 6);
}
bool has_texture_emissive()
{
    return buffer_material.properties & uint(1U << 7);
}
bool has_texture_occlusion()
{
    return buffer_material.properties & uint(1U << 8);
}
bool material_is_terrain()
{
    return buffer_material.properties & uint(1U << 9);
}
bool material_is_water()
{
    return buffer_material.properties & uint(1U << 10);
}

// frame properties
bool is_taa_enabled()
{
    return any(buffer_frame.taa_jitter_current);
}
bool is_ssr_enabled()
{
    return buffer_frame.options & uint(1U << 0);
}
bool is_ssgi_enabled()
{
    return buffer_frame.options & uint(1U << 1);
}
bool is_screen_space_shadows_enabled()
{
    return buffer_frame.options & uint(1U << 2);
}
bool is_fog_enabled()
{
    return buffer_frame.options & uint(1U << 3);
}
bool is_fog_volumetric_enabled()
{
    return buffer_frame.options & uint(1U << 4);
}

// pass properties
matrix pass_get_transform_previous()
{
    return buffer_pass.m_value;
}
float2 pass_get_resolution_in()
{
    return float2(buffer_pass.m_value._m03, buffer_pass.m_value._m22);
}
float2 pass_get_resolution_out()
{
    return float2(buffer_pass.m_value._m23, buffer_pass.m_value._m30);
}
float3 pass_get_f3_value()
{
    return float3(buffer_pass.m_value._m00, buffer_pass.m_value._m01, buffer_pass.m_value._m02);
}
float3 pass_get_f3_value2()
{
    return float3(buffer_pass.m_value._m20, buffer_pass.m_value._m21, buffer_pass.m_value._m31);
}
float4 pass_get_f4_value()
{
    return float4(buffer_pass.m_value._m10, buffer_pass.m_value._m11, buffer_pass.m_value._m12, buffer_pass.m_value._m13);
}
bool pass_is_transparent()
{
    return buffer_pass.m_value._m33;
}
bool pass_is_reflection_probe_available()
{
    return pass_get_f4_value().x == 1.0f;
} // this is more risky
bool pass_is_opaque()
{
    return !pass_is_transparent();
}
// m32 is free
