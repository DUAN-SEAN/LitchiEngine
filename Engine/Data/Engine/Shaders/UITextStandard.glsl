#shader vertex
#version 430 core

uniform mat4 u_mvp;

layout(location = 0) in  vec3 geo_Pos;
layout(location = 1) in  vec4 geo_color;
layout(location = 2) in  vec2 geo_TexCoords;

layout (std140) uniform EngineUBO
{
    mat4    ubo_Model;
    mat4    ubo_View;
    mat4    ubo_Projection;
    vec3    ubo_ViewPos;
    float   ubo_Time;
};

out VS_OUT
{
    vec4 Color;
    vec2 TexCoords;
} vs_out;

void main()
{
    vs_out.TexCoords = geo_TexCoords;
    vs_out.Color = geo_color;

    gl_Position = ubo_Projection * ubo_View * ubo_Model * vec4(geo_Pos, 1.0);
}


#shader fragment
#version 430 core

in VS_OUT
{
    vec4 Color;
    vec2 TexCoords;
} fs_in;

uniform sampler2D   u_DiffuseMap;

out vec4 FRAGMENT_COLOR;

void main()
{
    vec4 pixColor = texture(u_DiffuseMap,fs_in.TexCoords);
    FRAGMENT_COLOR = vec4(fs_in.Color.x,fs_in.Color.y,fs_in.Color.z,pixColor.r*fs_in.Color.a);
}