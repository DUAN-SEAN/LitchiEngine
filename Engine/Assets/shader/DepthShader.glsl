#shader vertex
#version 430 core

layout (location = 0) in vec3 geo_Pos;
layout (location = 1) in vec2 geo_TexCoords;
layout (location = 2) in vec3 geo_Normal;
layout (location = 3) in vec3 geo_Tangent;
layout (location = 4) in vec3 geo_Bitangent;

uniform mat4 ubo_LightSpaceMatrix;

layout (std140) uniform EngineUBO
{
    mat4    ubo_Model;
    mat4    ubo_View;
    mat4    ubo_Projection;
    vec3    ubo_ViewPos;
    float   ubo_Time;
};

void main()
{
	gl_Position = ubo_LightSpaceMatrix * ubo_Model * vec4(geo_Pos, 1.0f);
}

#shader fragment
#version 430 core

// Ouput data
out vec4 FRAGMENT_COLOR;

void main()
{
	FRAGMENT_COLOR = gl_FragCoord;
}