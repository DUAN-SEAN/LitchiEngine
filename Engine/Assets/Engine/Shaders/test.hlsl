
struct FrameBufferData
{
    matrix view;
    matrix projection;
    matrix view_projection;
    matrix view_projection_inverted;
    matrix view_projection_orthographic;
    matrix view_projection_unjittered;
    matrix view_projection_previous;

    float2 resolution_render;
    float2 resolution_output;

    float2 taa_jitter_current;
    float2 taa_jitter_previous;
    
    float delta_time;
    uint frame;
    float gamma;
    uint options;
    
    float3 camera_position;
    float camera_near;
    
    float3 camera_direction;
    float camera_far;
};

struct LightBufferData
{
    matrix view_projection[6];
    float4 intensity_range_angle_bias;
    float4 color;
    float4 position;
    float4 direction;

    float normal_bias;
    uint options;
    float2 padding;
    
    //float3 cascade_ends;
    //float padding2;
};

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

cbuffer BufferMaterial : register(b0)
{
    MaterialBufferData buffer_material;
}; // medium frequency - updates per material during the g-buffer pass


Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
Texture2D tex3 : register(t3);

// Global variable to store a combined view and projection transformation matrix,
// we initialize this variable from the application.
cbuffer GlobalMatrix : register(b1)
{
    matrix g_matrix;
    matrix g_matrix2;
}; // medium frequency - updates per material during the g-buffer pass
cbuffer BufferMaterial : register(b2)
{
    uniform float4 g_color;
}

int Const;
int Const2;
float color;
float2 color2;
float3 color3;
float4 color4;
//matrix matrix1;
//uniform MaterialBufferData buffer_material2;

struct Vertex_PosColor
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};
struct Pixel_PosColor
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct PixelOutputType
{
    float4 color : SV_Target0;
    // float fsr2_reactive_mask : SV_Target1;
};

Pixel_PosColor mainVS(Vertex_PosColor input)
{
    Pixel_PosColor output;

    input.position.w = 1.0f;
    output.color = input.color;
    
    return output;
}

PixelOutputType mainPS(Pixel_PosColor input)
{
    PixelOutputType output;

    output.color = input.color;
    // output.fsr2_reactive_mask = input.color.a;

    return output;
}