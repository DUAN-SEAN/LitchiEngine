
#pragma once

namespace LitchiRuntime
{
	/**
	* Data structure that defines the geometry of a vertex
	*/
	struct Vertex
	{
		float position[3];
		float texCoords[2];
		float normals[3];
		float tangent[3];
		float bitangent[3];

		// ����Ĺ�����Ϣ, Ȩ�غ͹�������
		float boneWeights[3];
		uint32_t boneIndices[4];
	};
}