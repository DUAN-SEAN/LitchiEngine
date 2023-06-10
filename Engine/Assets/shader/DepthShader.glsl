#shader vertex
#version 430 core

layout (location = 0) in vec3 geo_Pos;
layout (location = 1) in vec2 geo_TexCoords;
layout (location = 2) in vec3 geo_Normal;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(geo_Pos, 1.0f);
}

#shader fragment
#version 430 core

out vec4 FRAGMENT_COLOR;

void main()
{
	 // gl_FragDepth = gl_FragCoord.z;
}