#shader vertex
#version 430 core

layout (location = 0) in vec3 geo_Pos;
layout (location = 1) in vec2 geo_TexCoords;
layout (location = 2) in vec3 geo_Normal;
layout (location = 3) in vec3 geo_Tangent;
layout (location = 4) in vec3 geo_Bitangent;
layout (location = 5) in ivec4 geo_boneIdArr;
layout (location = 6) in vec3 geo_boneWeightArr;

layout (std140) uniform EngineUBO
{
    mat4    ubo_Model;
    mat4    ubo_View;
    mat4    ubo_Projection;
    vec3    ubo_ViewPos;
    float   ubo_Time;
};

uniform mat4 ubo_LightSpaceMatrix;

const int MAX_BONES = 250;

uniform mat4 ubo_boneFinalMatrixArr[MAX_BONES];

void main()
{ 
    /*Bone Animation*/
	vec4 weights = vec4(geo_boneWeightArr.x, geo_boneWeightArr.y, geo_boneWeightArr.z, 1.0f - geo_boneWeightArr.x - geo_boneWeightArr.y - geo_boneWeightArr.z);
    mat4 boneTransform = ubo_boneFinalMatrixArr[geo_boneIdArr[0]] * weights[0];
    boneTransform += ubo_boneFinalMatrixArr[geo_boneIdArr[1]] * weights[1];
    boneTransform += ubo_boneFinalMatrixArr[geo_boneIdArr[2]] * weights[2];
    boneTransform += ubo_boneFinalMatrixArr[geo_boneIdArr[3]] * weights[3];
    
    vec4 PosL = boneTransform * vec4(geo_Pos, 1.0);

	gl_Position = ubo_LightSpaceMatrix * ubo_Model * PosL;
}

#shader fragment
#version 430 core

// Ouput data
out vec4 FRAGMENT_COLOR;

void main()
{
	// FRAGMENT_COLOR = gl_FragCoord;
}