
#pragma once

//= INCLUDES =====
#include <cstdint>
//================

namespace LitchiRuntime
{
    #define DEBUG_COLOR Vector4(0.41f, 0.86f, 1.0f, 1.0f)

    // we are using double buffering so 5 is enough
    constexpr uint8_t resources_frame_lifetime = 5;
    constexpr uint8_t renderer_path_max = 10;

    enum class Renderer_Option : uint32_t
    {
        Aabb,
        PickingRay,
        Grid,
        TransformHandle,
        SelectionOutline,
        Lights,
        PerformanceMetrics,
        Physics,
        Wireframe,
        Bloom,
        Fog,
        FogVolumetric,
        ScreenSpaceGlobalIllumination,
        ScreenSpaceShadows,
        ScreenSpaceReflections,
        MotionBlur,
        DepthOfField,
        FilmGrain,
        ChromaticAberration,
        Anisotropy,
        ShadowResolution,
        Exposure,
        WhitePoint,
        Antialiasing,
        Tonemapping,
        Upsampling,
        Sharpness,
        Hdr,
        Vsync,
        VariableRateShading,
        ResolutionScale,
        DynamicResolution,
        Max
    };

    enum class Renderer_Antialiasing : uint32_t
    {
        Disabled,
        Fxaa,
        Taa,
        TaaFxaa
    };

    enum class Renderer_Tonemapping : uint32_t
    {
        Amd,
        Aces,
        Reinhard,
        Uncharted2,
        Matrix,
        Disabled,
    };

    enum class Renderer_Upsampling : uint32_t
    {
        Linear,
        Fsr2
    };

    enum class Renderer_BindingsCb
    {
        frame    = 0,
        light    = 1,
        material = 2,
        lightArr = 3,
        boneArr = 4,
        rendererPath = 5,
    };
    
    enum class Renderer_BindingsSrv
    {
        // Material
        material_albedo    = 0,
        material_roughness = 1,
        material_metallic  = 2,
        material_normal    = 3,
        material_height    = 4,
        material_occlusion = 5,
        material_emission  = 6,
        material_mask      = 7,
    
        // G-buffer
        gbuffer_albedo            = 8,
        gbuffer_normal            = 9,
        gbuffer_material          = 10,
        gbuffer_material_2        = 11,
        gbuffer_velocity          = 12,
        gbuffer_velocity_previous = 13,
        gbuffer_depth             = 14,
    
        // Lighting
        light_diffuse              = 15,
        light_diffuse_transparent  = 16,
        light_specular             = 17,
        light_specular_transparent = 18,
        light_volumetric           = 19,
    
        // Light depth/color maps
        light_directional_depth = 20,
        light_directional_color = 21,
        light_point_depth       = 22,
        light_point_color       = 23,
        light_spot_depth        = 24,
        light_spot_color        = 25,
    
        // Noise
        noise_normal = 26,
        noise_blue   = 27,
    
        // Misc
        lutIbl           = 28,
        environment      = 31,
        ssgi             = 32,
        ssr              = 33,
        frame            = 34,
        tex              = 35,
        tex2             = 36,
        font_atlas       = 37,
        reflection_probe = 38,

        //Sky Box
        tex_skyBox = 40
    };

    enum class Renderer_BindingsUav
    {
        tex            = 0,
        tex2           = 1,
        tex3           = 2,
        atomic_counter = 3,
        tex_array      = 4
    };

    enum class Renderer_Shader : uint8_t
    {
        forward_v,
        forward_p,
        depth_light_V,
        depth_light_p,
        depth_light_skin_V,
        depth_light_skin_p,
        quad_v,
        quad_p,
        line_v,
        line_p,
        skybox_v,
        skybox_p,
        font_v,
        font_p,
        ui_image_v,
        ui_image_p,
        max,
    };
    
    enum class Renderer_RenderTarget : uint8_t
    {
        undefined,
        frame_render,
        frame_render_2,
        frame_output,
        frame_output_2,
        max
    };
    
    enum class Renderer_Entity
    {
        Geometry,
        GeometryTransparent,
        SkinGeometry,
        SkinGeometryTransparent,
        Light,
        Camera,
        ReflectionProbe,
        AudioSource,
        UI,
        Canvas,
    };

    enum class Renderer_Sampler
    {
        Compare_depth,
        Point_clamp_edge,
        Point_clamp_border,
        Point_wrap,
        Bilinear_clamp_edge,
        Bilienar_clamp_border,
        Bilinear_wrap,
        Trilinear_clamp,
        Anisotropic_wrap,
        Max
    };

    enum class Renderer_ConstantBuffer
    {
        Frame,
        LightArr,
        RendererPath,
        Max
    };

    enum class Renderer_StructuredBuffer
    {
        Spd,
        Lights
    };

    enum class Renderer_StandardTexture
    {
        Noise_normal,
        Noise_blue,
        White,
        Black,
        Transparent,
        Checkerboard,
        Gizmo_light_directional,
        Gizmo_light_point,
        Gizmo_light_spot,
        Gizmo_audio_source,
        SkyBox
    };

    enum class Renderer_MeshType
    {
        NotAssigned,
        Cube,
        Quad,
        Grid,
        Sphere,
        Cylinder,
        Cone,
        Custom
    };

    enum class Renderer_RasterizerState
    {
        Solid_cull_back,
        Wireframe_cull_none,
        Solid_cull_none,
        Light_point_spot,
        Light_directional
    };

    enum class Renderer_DepthStencilState
    {
        Off,
        Stencil_read,
        Depth_read_write_stencil_read,
        Depth_read,
        Depth_read_write_stencil_write
    };

    enum class Renderer_BlendState
    {
        Disabled,
        Alpha,
        Additive
    };
}
