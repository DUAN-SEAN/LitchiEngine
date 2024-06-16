
#ifndef LITCHI_COMMON
#define LITCHI_COMMON

//= INCLUDES ======================
#include "common_vertex_pixel.hlsl"
#include "common_buffers.hlsl"
#include "common_samplers.hlsl"
#include "common_textures.hlsl"
//=================================

/*------------------------------------------------------------------------------
    CONSTANTS
------------------------------------------------------------------------------*/
static const float PI                  = 3.14159265f;
static const float PI2                 = 6.28318530f;
static const float PI4                 = 12.5663706f;
static const float INV_PI              = 0.31830988f;
static const float PI_HALF             = PI * 0.5f;
static const float FLT_MIN             = 0.00000001f;
static const float FLT_MAX_10          = 511.0f;
static const float FLT_MAX_11          = 1023.0f;
static const float FLT_MAX_14          = 8191.0f;
static const float FLT_MAX_16          = 32767.0f;
static const float FLT_MAX_16U         = 65535.0f;
static const float ALPHA_THRESHOLD     = 0.6f;
static const float RPC_9               = 0.11111111111f;
static const float RPC_16              = 0.0625f;
static const float RPC_32              = 0.03125f;
static const float ENVIRONMENT_MAX_MIP = 11.0f;
static const uint THREAD_GROUP_COUNT_X = 8;
static const uint THREAD_GROUP_COUNT_Y = 8;
static const uint THREAD_GROUP_COUNT   = 64;

/*------------------------------------------------------------------------------
   COMMON
------------------------------------------------------------------------------*/
float2 get_rt_texel_size()          { return float2(1.0f / pass_get_resolution_out().x, 1.0f /pass_get_resolution_out().y); }
float2 get_tex_noise_normal_scale() { return float2(buffer_frame.resolution_render.x / 256.0f, buffer_frame.resolution_render.y / 256.0f); }
float2 get_tex_noise_blue_scale()   { return float2(buffer_frame.resolution_render.x / 470.0f, buffer_frame.resolution_render.y / 470.0f); }
float3 degamma(float3 color)        { return pow(color, buffer_frame.gamma); }
float3 gamma(float3 color)          { return pow(color, 1.0f / buffer_frame.gamma); }

/*------------------------------------------------------------------------------
    MATH
------------------------------------------------------------------------------*/
float min2(float2 value)                                { return min(value.x, value.y); }
float min3(float3 value)                                { return min(min(value.x, value.y), value.z); }
float min3(float a, float b, float c)                   { return min(min(a, b), c); }
float min4(float a, float b, float c, float d)          { return min(min(min(a, b), c), d); }
float min5(float a, float b, float c, float d, float e) { return min(min(min(min(a, b), c), d), e); }

float max2(float2 value)                                { return max(value.x, value.y); }
float max3(float3 value)                                { return max(max(value.x, value.y), value.z); }
float max4(float a, float b, float c, float d)          { return max(max(max(a, b), c), d); }
float max5(float a, float b, float c, float d, float e) { return max(max(max(max(a, b), c), d), e); }

float pow2(float x)
{
    return x * x;
}

float pow3(float x)
{
    float xx = x*x;
    return xx * x;
}

float pow4(float x)
{
    float xx = x*x;
    return xx * xx;
}

bool is_saturated(float value)  { return value == saturate(value); }
bool is_saturated(float2 value) { return is_saturated(value.x) && is_saturated(value.y); }
bool is_saturated(float3 value) { return is_saturated(value.x) && is_saturated(value.y) && is_saturated(value.z); }
bool is_saturated(float4 value) { return is_saturated(value.x) && is_saturated(value.y) && is_saturated(value.z) && is_saturated(value.w); }

bool is_valid_uv(float2 value) { return (value.x >= 0.0f && value.x <= 1.0f) || (value.y >= 0.0f && value.y <= 1.0f); }

/*------------------------------------------------------------------------------
    SATURATE
------------------------------------------------------------------------------*/
float  saturate_11(float x)  { return clamp(x, 0.0f, FLT_MAX_11); }
float2 saturate_11(float2 x) { return clamp(x, 0.0f, FLT_MAX_11); }
float3 saturate_11(float3 x) { return clamp(x, 0.0f, FLT_MAX_11); }
float4 saturate_11(float4 x) { return clamp(x, 0.0f, FLT_MAX_11); }

float  saturate_16(float x)  { return clamp(x, 0.0f, FLT_MAX_16); }
float2 saturate_16(float2 x) { return clamp(x, 0.0f, FLT_MAX_16); }
float3 saturate_16(float3 x) { return clamp(x, 0.0f, FLT_MAX_16); }
float4 saturate_16(float4 x) { return clamp(x, 0.0f, FLT_MAX_16); }

/*------------------------------------------------------------------------------
    PACKING/UNPACKING
------------------------------------------------------------------------------*/
float3 unpack(float3 value) { return value * 2.0f - 1.0f; }
float3 pack(float3 value)   { return value * 0.5f + 0.5f; }
float2 unpack(float2 value) { return value * 2.0f - 1.0f; }
float2 pack(float2 value)   { return value * 0.5f + 0.5f; }
float  unpack(float value)  { return value * 2.0f - 1.0f; }
float  pack(float value)    { return value * 0.5f + 0.5f; }

float pack_uint32_to_float16(uint i)    { return (float)i / FLT_MAX_16; }
uint  unpack_float16_to_uint32(float f) { return round(f * FLT_MAX_16); }

float pack_float_int(float f, uint i, uint numBitI, uint numBitTarget)
{
    // Constant optimize by compiler
    float precision         = float(1U << numBitTarget);
    float maxi              = float(1U << numBitI);
    float precisionMinusOne = precision - 1.0;
    float t1                = ((precision / maxi) - 1.0) / precisionMinusOne;
    float t2                = (precision / maxi) / precisionMinusOne;

    // Code
    return t1 * f + t2 * float(i);
}

void unpack_float_int(float val, uint numBitI, uint numBitTarget, out float f, out uint i)
{
    // Constant optimize by compiler
    float precision         = float(1U << numBitTarget);
    float maxi              = float(1U << numBitI);
    float precisionMinusOne = precision - 1.0;
    float t1                = ((precision / maxi) - 1.0) / precisionMinusOne;
    float t2                = (precision / maxi) / precisionMinusOne;

    // Code
    // extract integer part
    // + rcp(precisionMinusOne) to deal with precision issue
    i = int((val / t2) + rcp(precisionMinusOne));
    // Now that we have i, solve formula in PackFloatInt for f
    //f = (val - t2 * float(i)) / t1 => convert in mads form
    f = saturate((-t2 * float(i) + val) / t1); // Saturate in case of precision issue
}

/*------------------------------------------------------------------------------
    FAST MATH APPROXIMATIONS
------------------------------------------------------------------------------*/

// Relative error : < 0.7% over full
// Precise format : ~small float
// 1 ALU
float fast_sqrt(float x)
{
    int i = asint(x);
    i = 0x1FBD1DF5 + (i >> 1);
    return asfloat(i);
}

float fast_length(float3 v)
{
    float LengthSqr = dot(v, v);
    return fast_sqrt(LengthSqr);
}

float fast_sin(float x)
{
    const float B = 4 / PI;
    const float C = -4 / PI2;
    const float P = 0.225;

    float y = B * x + C * x * abs(x);
    y = P * (y * abs(y) - y) + y;
    return y;
}

float fast_cos(float x)
{
   return abs(abs(x)  /PI2 % 4 - 2) - 1;
}

/*------------------------------------------------------------------------------
    TRANSFORMATIONS
------------------------------------------------------------------------------*/
float3 world_to_view(float3 x, bool is_position = true)
{
    return mul(float4(x, (float)is_position), buffer_rendererPath.view).xyz;
}

float3 world_to_ndc(float3 x, bool is_position = true)
{
    float4 ndc = mul(float4(x, (float) is_position), buffer_rendererPath.view_projection);
    return ndc.xyz / ndc.w;
}

float3 world_to_ndc(float3 x, float4x4 transform) // shadow mapping
{
    float4 ndc = mul(float4(x, 1.0f), transform);
    return ndc.xyz / ndc.w;
}

float3 view_to_ndc(float3 x, bool is_position = true)
{
    float4 ndc = mul(float4(x, (float) is_position), buffer_rendererPath.projection);
    return ndc.xyz / ndc.w;
}

float2 world_to_uv(float3 x, bool is_position = true)
{
    float4 uv = mul(float4(x, (float) is_position), buffer_rendererPath.view_projection);
    return (uv.xy / uv.w) * float2(0.5f, -0.5f) + 0.5f;
}

float2 view_to_uv(float3 x, bool is_position = true)
{
    float4 uv = mul(float4(x, (float) is_position), buffer_rendererPath.projection);
    return (uv.xy / uv.w) * float2(0.5f, -0.5f) + 0.5f;
}

float2 ndc_to_uv(float2 x)
{
    return x * float2(0.5f, -0.5f) + 0.5f;
}

float2 ndc_to_uv(float3 x)
{
    return x.xy * float2(0.5f, -0.5f) + 0.5f;
}

float3 get_position_ws_from_depth(const float2 uv, const float depth)
{
    float x          = uv.x * 2.0f - 1.0f;
    float y          = (1.0f - uv.y) * 2.0f - 1.0f;
    float4 pos_clip  = float4(x, y, depth, 1.0f);
    float4 pos_world = mul(pos_clip, buffer_rendererPath.view_projection_inverted);
    return             pos_world.xyz / pos_world.w;
}

/*------------------------------------------------------------------------------
    NORMAL
------------------------------------------------------------------------------*/
// Reconstruct normal Z, X and Y components have to be in a -1 to 1 range.
float3 reconstruct_normal_z(float2 normal)
{
    float z = sqrt(saturate(1.0f - dot(normal, normal)));
    return float3(normal, z);
}

// float3 get_normal(uint2 pos)
// {
    // // Load returns 0 for any value accessed out of bounds, so clamp.
    // pos.x = clamp(pos.x, 0, buffer_frame.resolution_render.x);
    // pos.y = clamp(pos.y, 0, buffer_frame.resolution_render.y);
    
    // return tex_normal[pos].xyz;
// }

// float3 get_normal(float2 uv)
// {
    // return tex_normal.SampleLevel(samplers[sampler_point_clamp], uv, 0).xyz;
// }

// float3 get_normal_view_space(uint2 pos)
// {
    // return normalize(mul(float4(get_normal(pos), 0.0f), buffer_rendererPath.view).xyz);
// }

// float3 get_normal_view_space(float2 uv)
// {
    // return normalize(mul(float4(get_normal(uv), 0.0f), buffer_rendererPath.view).xyz);
// }

float3x3 make_tangent_to_world_matrix(float3 n, float3 t)
{
    // re-orthogonalize T with respect to N
    t = normalize(t - dot(t, n) * n);
    // compute bitangent
    float3 b = cross(n, t);
    // create matrix
    return float3x3(t, b, n); 
}

float3x3 make_world_to_tangent_matrix(float3 n, float3 t)
{
    return transpose(make_tangent_to_world_matrix(n, t));
}

/*------------------------------------------------------------------------------
    DEPTH
------------------------------------------------------------------------------*/
float get_depth(uint2 position)
{
     // out of bounds check
    position = clamp(position, uint2(0, 0), uint2(buffer_frame.resolution_render) - uint2(1, 1));
    return tex_depth[position].r;
}

float get_depth(float2 uv)
{
     // effects like screen space shadows, can get artefacts if a point sampler is used
    return tex_depth.SampleLevel(samplers[sampler_bilinear_clamp], uv, 0).r;
}

float get_linear_depth(float z, float near, float far)
{
    float z_b = z;
    float z_n = 2.0f * z_b - 1.0f;
    return 2.0f * far * near / (near + far - z_n * (near - far));
}

float get_linear_depth(float z)
{
    return get_linear_depth(z, buffer_rendererPath.camera_near, buffer_rendererPath.camera_far);
}

// float get_linear_depth(uint2 pos)
// {
    // return get_linear_depth(get_depth(pos));
// }

// float get_linear_depth(float2 uv)
// {
    // return get_linear_depth(get_depth(uv));
// }

/*------------------------------------------------------------------------------
    POSITION
------------------------------------------------------------------------------*/
float3 get_position(float z, float2 uv)
{
    float x             = uv.x * 2.0f - 1.0f;
    float y             = (1.0f - uv.y) * 2.0f - 1.0f;
    float4 pos_clip     = float4(x, y, z, 1.0f);
    float4 pos_world = mul(pos_clip, buffer_rendererPath.view_projection_inverted);
    return pos_world.xyz / pos_world.w;
}

float3 get_position(float2 uv)
{
    return get_position(get_depth(uv), uv);
}

float3 get_position(uint2 pos)
{
    const float2 uv = (pos + 0.5f) / pass_get_resolution_out();
    return get_position(get_depth(pos), uv);
}

float3 get_position_view_space(uint2 pos)
{
    return mul(float4(get_position(pos), 1.0f), buffer_rendererPath.view).xyz;
}

float3 get_position_view_space(float2 uv)
{
    return mul(float4(get_position(uv), 1.0f), buffer_rendererPath.view).xyz;
}

/*------------------------------------------------------------------------------
    VIEW DIRECTION
------------------------------------------------------------------------------*/
float3 get_view_direction(float3 position_world)
{
    return normalize(position_world - buffer_rendererPath.camera_position.xyz);
}

float3 get_view_direction(float depth, float2 uv)
{
    return get_view_direction(get_position(depth, uv));
}

float3 get_view_direction(float2 uv)
{
    return get_view_direction(get_position(uv));
}

float3 get_view_direction(uint2 pos)
{
    const float2 uv = (pos + 0.5f) / pass_get_resolution_out();
    return get_view_direction(uv);
}

float3 get_view_direction_view_space(float2 uv)
{
    return mul(float4(get_view_direction(get_position(uv)), 0.0f), buffer_rendererPath.view).xyz;
}

float3 get_view_direction_view_space(uint2 pos)
{
    const float2 uv = (pos + 0.5f) / pass_get_resolution_out();
    return get_view_direction_view_space(uv);
}

float3 get_view_direction_view_space(float3 position_world)
{
    return mul(float4(get_view_direction(position_world), 0.0f), buffer_rendererPath.view).xyz;
}

/*------------------------------------------------------------------------------
    DIRECTION UV
------------------------------------------------------------------------------*/
float2 direction_sphere_uv(float3 direction)
{
    float n = length(direction.xz);
    float2 uv = float2((n > 0.0000001) ? direction.x / n : 0.0, direction.y);
    uv = acos(uv) * INV_PI;
    uv.x = (direction.z > 0.0) ? uv.x * 0.5 : 1.0 - (uv.x * 0.5);
    uv.x = 1.0 - uv.x;
    
    return uv;
}

uint direction_to_cube_face_index(const float3 direction)
{
    float3 direction_abs = abs(direction);
    float max_coordinate = max3(direction_abs);
    
    if (max_coordinate == direction_abs.x)
    {
        return direction_abs.x == direction.x ? 0 : 1;
    }
    else if (max_coordinate == direction_abs.y)
    {
        return direction_abs.y == direction.y ? 2 : 3;
    }
    else
    {
        return direction_abs.z == direction.z ? 4 : 5;
    }
    
    return 0;
}

/*------------------------------------------------------------------------------
    LUMINANCE
------------------------------------------------------------------------------*/
static const float3 srgb_color_space_coefficient = float3(0.299f, 0.587f, 0.114f);

float luminance(float3 color)
{
    return max(dot(color, srgb_color_space_coefficient), FLT_MIN);
}

float luminance(float4 color)
{
    return max(dot(color.rgb, srgb_color_space_coefficient), FLT_MIN);
}

/*------------------------------------------------------------------------------
    NOISE/RANDOM
------------------------------------------------------------------------------*/
float get_random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

// An expansion on the interleaved gradient function from Jimenez 2014 http://goo.gl/eomGso
float get_noise_interleaved_gradient(float2 screen_pos, bool animate, bool animate_even_with_taa_off)
{
    // temporal factor
    float animate_    = saturate((float)is_taa_enabled() + (float)animate_even_with_taa_off) * (float)animate;
    float frame_count = (float)buffer_frame.frame;
    float frame_step  = float(frame_count % 16) * RPC_16 * animate_;
    screen_pos.x     += frame_step * 4.7526;
    screen_pos.y     += frame_step * 3.1914;

    float3 magic = float3(0.06711056f, 0.00583715f, 52.9829189f);
    return frac(magic.z * frac(dot(screen_pos, magic.xy)));
}

// float get_noise_blue(float2 screen_pos)
// {
    // // Temporal factor - animated blue noise image
    // //float taaOn      = (float)is_taa_enabled();
    // //float frameCount = (float)g_frame;
    // //float frameStep  = taaOn * float(frameCount % 16) * RPC_16;
    // //screen_pos.x     += frameStep * 4.7526;
    // //screen_pos.y    += frameStep * 3.1914;

    // // Temporal factor - alternate between blue noise images
    // float slice = (buffer_frame.frame % 8) * (float)is_taa_enabled();

    // float2 uv = (screen_pos + 0.5f) * get_tex_noise_blue_scale();
    // return tex_noise_blue.SampleLevel(samplers[sampler_point_wrap], float3(uv.x, uv.y, slice), 0).r;
// }

// float3 get_noise_normal(uint2 screen_pos)
// {
    // float2 uv = (screen_pos + 0.5f) * get_tex_noise_normal_scale();
    // return normalize(tex_noise_normal.SampleLevel(samplers[sampler_point_wrap], uv, 0).xyz);
// }

/*------------------------------------------------------------------------------
    OCCLUSION/SHADOWING
------------------------------------------------------------------------------*/
// The Technical Art of Uncharted 4 - http://advances.realtimerendering.com/other/2016/naughty_dog/index.html
float microw_shadowing_nt(float n_dot_l, float ao)
{
    float aperture = 2.0f * ao * ao;
    return saturate(abs(n_dot_l) + aperture - 1.0f);
}

// Chan 2018, "Material Advances in Call of Duty: WWII"
float microw_shadowing_cod(float n_dot_l, float visibility)
{
    float aperture = rsqrt(1.0 - visibility);
    float microShadow = saturate(n_dot_l * aperture);
    return microShadow * microShadow;
}

float3 project_onto_paraboloid(float3 light_to_vertex_view, float near_plane, float far_plane)
{
    float3 ndc = 0.0f;

    // normalize the light to vertex vector
    float d = length(light_to_vertex_view);
    light_to_vertex_view /= d;

    // project onto paraboloid
    ndc.xy = light_to_vertex_view.xy / (light_to_vertex_view.z + 1.0f);

     // calculate reverse depth
    ndc.z = (far_plane - d) / (far_plane - near_plane);

    // if the vertex is behind the light, clamp it to the edge of the circular paraboloid
    float is_valid = step(0.0f, light_to_vertex_view.z);
    float radius_squared = dot(ndc.xy, ndc.xy);
    float clamped_radius = sqrt(clamp(radius_squared, 0.0f, 1.0f));
    ndc.xy = is_valid * ndc.xy + (1.0f - is_valid) * (ndc.xy / clamped_radius);

    return ndc;
}
/*------------------------------------------------------------------------------
    PRIMITIVES
------------------------------------------------------------------------------*/
float draw_line(float2 p1, float2 p2, float2 uv, float a)
{
    float r = 0.0f;
    float one_px = 1. / pass_get_resolution_out().x; // not really one px

    // get dist between points
    float d = distance(p1, p2);

    // get dist between current pixel and p1
    float duv = distance(p1, uv);

    // if point is on line, according to dist, it should match current uv 
    r = 1.0f - floor(1.0f - (a * one_px) + distance(lerp(p1, p2, clamp(duv / d, 0.0f, 1.0f)), uv));

    return r;
}

float draw_line_view_space(float3 p1, float3 p2, float2 uv, float a)
{
    float2 p1_uv = view_to_uv(p1);
    float2 p2_uv = view_to_uv(p2);
    return draw_line(p1_uv, p2_uv, uv, a);
}

float draw_circle(float2 origin, float radius, float2 uv)
{
    return (distance(origin, uv) <= radius) ? 1.0f : 0.0f;
}

float draw_circle_view_space(float3 origin, float radius, float2 uv)
{
    float2 origin_uv = view_to_uv(origin);
    return draw_circle(origin_uv, radius, uv);
}

/*------------------------------------------------------------------------------
    MISC
------------------------------------------------------------------------------*/
float3 compute_diffuse_energy(float3 F, float metallic)
{
    float3 kS = F;          // The energy of light that gets reflected - Equal to Fresnel
    float3 kD = 1.0f - kS;  // Remaining energy, light that gets refracted
    kD *= 1.0f - metallic;  // Multiply kD by the inverse metalness such that only non-metals have diffuse lighting
    
    return kD;
}

float screen_fade(float2 uv)
{
    float2 fade = max(0.0f, 12.0f * abs(uv - 0.5f) - 5.0f);
    return saturate(1.0f - dot(fade, fade));
}

// Find good arbitrary axis vectors to represent U and V axes of a plane,
// given just the normal. Ported from UnMath.h
void find_best_axis_vectors(float3 In, out float3 Axis1, out float3 Axis2)
{
    const float3 N = abs(In);

    // Find best basis vectors.
    if (N.z > N.x && N.z > N.y)
    {
        Axis1 = float3(1, 0, 0);
    }
    else
    {
        Axis1 = float3(0, 0, 1);
    }

    Axis1 = normalize(Axis1 - In * dot(Axis1, In));
    Axis2 = cross(Axis1, In);
}

static const float3 hemisphere_samples[64] =
{
    float3(0.04977, -0.04471, 0.04996),
    float3(0.01457, 0.01653, 0.00224),
    float3(-0.04065, -0.01937, 0.03193),
    float3(0.01378, -0.09158, 0.04092),
    float3(0.05599, 0.05979, 0.05766),
    float3(0.09227, 0.04428, 0.01545),
    float3(-0.00204, -0.0544, 0.06674),
    float3(-0.00033, -0.00019, 0.00037),
    float3(0.05004, -0.04665, 0.02538),
    float3(0.03813, 0.0314, 0.03287),
    float3(-0.03188, 0.02046, 0.02251),
    float3(0.0557, -0.03697, 0.05449),
    float3(0.05737, -0.02254, 0.07554),
    float3(-0.01609, -0.00377, 0.05547),
    float3(-0.02503, -0.02483, 0.02495),
    float3(-0.03369, 0.02139, 0.0254),
    float3(-0.01753, 0.01439, 0.00535),
    float3(0.07336, 0.11205, 0.01101),
    float3(-0.04406, -0.09028, 0.08368),
    float3(-0.08328, -0.00168, 0.08499),
    float3(-0.01041, -0.03287, 0.01927),
    float3(0.00321, -0.00488, 0.00416),
    float3(-0.00738, -0.06583, 0.0674),
    float3(0.09414, -0.008, 0.14335),
    float3(0.07683, 0.12697, 0.107),
    float3(0.00039, 0.00045, 0.0003),
    float3(-0.10479, 0.06544, 0.10174),
    float3(-0.00445, -0.11964, 0.1619),
    float3(-0.07455, 0.03445, 0.22414),
    float3(-0.00276, 0.00308, 0.00292),
    float3(-0.10851, 0.14234, 0.16644),
    float3(0.04688, 0.10364, 0.05958),
    float3(0.13457, -0.02251, 0.13051),
    float3(-0.16449, -0.15564, 0.12454),
    float3(-0.18767, -0.20883, 0.05777),
    float3(-0.04372, 0.08693, 0.0748),
    float3(-0.00256, -0.002, 0.00407),
    float3(-0.0967, -0.18226, 0.29949),
    float3(-0.22577, 0.31606, 0.08916),
    float3(-0.02751, 0.28719, 0.31718),
    float3(0.20722, -0.27084, 0.11013),
    float3(0.0549, 0.10434, 0.32311),
    float3(-0.13086, 0.11929, 0.28022),
    float3(0.15404, -0.06537, 0.22984),
    float3(0.05294, -0.22787, 0.14848),
    float3(-0.18731, -0.04022, 0.01593),
    float3(0.14184, 0.04716, 0.13485),
    float3(-0.04427, 0.05562, 0.05586),
    float3(-0.02358, -0.08097, 0.21913),
    float3(-0.14215, 0.19807, 0.00519),
    float3(0.15865, 0.23046, 0.04372),
    float3(0.03004, 0.38183, 0.16383),
    float3(0.08301, -0.30966, 0.06741),
    float3(0.22695, -0.23535, 0.19367),
    float3(0.38129, 0.33204, 0.52949),
    float3(-0.55627, 0.29472, 0.3011),
    float3(0.42449, 0.00565, 0.11758),
    float3(0.3665, 0.00359, 0.0857),
    float3(0.32902, 0.0309, 0.1785),
    float3(-0.08294, 0.51285, 0.05656),
    float3(0.86736, -0.00273, 0.10014),
    float3(0.45574, -0.77201, 0.00384),
    float3(0.41729, -0.15485, 0.46251),
    float3(-0.44272, -0.67928, 0.1865)
};

//= INCLUDES =================
#include "common_structs.hlsl"
//============================

#endif // SPARTAN_COMMON
