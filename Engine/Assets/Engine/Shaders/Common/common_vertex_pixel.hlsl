
// Vertex
struct Vertex_Pos
{
    float4 position : POSITION0;
};

struct Vertex_PosUv
{
    float4 position : POSITION0;
    float2 uv       : TEXCOORD0;
};

struct Vertex_PosColor
{
    float4 position : POSITION0;
    float4 color    : COLOR0;
};

struct Vertex_Pos2dUvColor
{
    float2 position : POSITION0;
    float2 uv       : TEXCOORD0;
    float4 color    : COLOR0;
};

struct Vertex_PosUvNorTan
{
    float4 position           : POSITION0;
    float2 uv                 : TEXCOORD0;
    float3 normal             : NORMAL0;
    float3 tangent            : TANGENT0;
    #if INSTANCED
    matrix instance_transform : INSTANCE_TRANSFORM0;
    #endif
};

// Pixel
struct Pixel_Pos
{
    float4 position : SV_POSITION;
};

struct Pixel_PosUv
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

struct Pixel_PosColor
{
    float4 position : SV_POSITION;
    float4 color    : COLOR0;
};


struct Pixel_PosNor
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
};

struct Pixel_PosColUv
{
    float4 position : SV_POSITION;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD;
};

struct Pixel_PosUvNorTan
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
};
