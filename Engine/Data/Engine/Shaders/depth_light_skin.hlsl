
//= INCLUDES =========
#include "Common/common.hlsl"
//====================

Pixel_PosUv mainVS(Vertex_PosUvNorTanBone input)
{
    Pixel_PosUv output;

    float4 weights = float4(input.boneWeights.x, input.boneWeights.y, input.boneWeights.z,
    1.0f - input.boneWeights.x - input.boneWeights.y - input.boneWeights.z);
    matrix boneTransform = bone_data_arr.boneTransformArr[(int) input.boneIndices[0]] * weights[0];
    boneTransform += bone_data_arr.boneTransformArr[(int) input.boneIndices[1]] * weights[1];
    boneTransform += bone_data_arr.boneTransformArr[(int) input.boneIndices[2]] * weights[2];
    boneTransform += bone_data_arr.boneTransformArr[(int) input.boneIndices[3]] * weights[3];
    
    input.position.w = 1.0f;
    float4 posL = mul(input.position, boneTransform);

    output.position = mul(posL, buffer_pass.transform);
    // output.position      = mul(output.position, buffer_light.view_projection[0]);

    output.uv = input.uv;

    return output;
}

// transparent/colored shadows
float4 mainPS(Pixel_PosUv input) : SV_TARGET
{
    return input.position;
}
