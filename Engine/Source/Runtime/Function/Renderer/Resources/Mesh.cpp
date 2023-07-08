
#include <algorithm>

#include "Mesh.h"

LitchiRuntime::Mesh::Mesh(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex, const bool isSkinned) :
	m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
	m_indicesCount(static_cast<uint32_t>(p_indices.size())),
	m_materialIndex(p_materialIndex),
	m_isSkinned(isSkinned)
{
	vertices = p_vertices;
	indices = p_indices;
	CreateBuffers(vertices, indices);
	ComputeBoundingSphere(p_vertices);
}

void LitchiRuntime::Mesh::Bind()
{
	m_vertexArray.Bind();
}

void LitchiRuntime::Mesh::Unbind()
{
	m_vertexArray.Unbind();
}

uint32_t LitchiRuntime::Mesh::GetVertexCount()
{
	return m_vertexCount;
}

uint32_t LitchiRuntime::Mesh::GetIndexCount()
{
	return m_indicesCount;
}

uint32_t LitchiRuntime::Mesh::GetMaterialIndex() const
{
	return m_materialIndex;
}

const LitchiRuntime::BoundingSphere& LitchiRuntime::Mesh::GetBoundingSphere() const
{
	return m_boundingSphere;
}

void LitchiRuntime::Mesh::CreateBuffers(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices)
{
	std::vector<float> vertexData;

	std::vector<unsigned int> rawIndices;

	for (const auto& vertex : p_vertices)
	{
		vertexData.push_back(vertex.position[0]);
		vertexData.push_back(vertex.position[1]);
		vertexData.push_back(vertex.position[2]);

		vertexData.push_back(vertex.texCoords[0]);
		vertexData.push_back(vertex.texCoords[1]);

		vertexData.push_back(vertex.normals[0]);
		vertexData.push_back(vertex.normals[1]);
		vertexData.push_back(vertex.normals[2]);

		vertexData.push_back(vertex.tangent[0]);
		vertexData.push_back(vertex.tangent[1]);
		vertexData.push_back(vertex.tangent[2]);

		vertexData.push_back(vertex.bitangent[0]);
		vertexData.push_back(vertex.bitangent[1]);
		vertexData.push_back(vertex.bitangent[2]);

		if (m_isSkinned)
		{
			vertexData.push_back(vertex.boneIndices[0]);
			vertexData.push_back(vertex.boneIndices[1]);
			vertexData.push_back(vertex.boneIndices[2]);
			vertexData.push_back(vertex.boneIndices[3]);

			vertexData.push_back(vertex.boneWeights[0]);
			vertexData.push_back(vertex.boneWeights[1]);
			vertexData.push_back(vertex.boneWeights[2]);
		}
	}

	// todo 大小不对，需要修改
	m_vertexBuffer = std::make_unique<VertexBuffer<float>>(vertexData);
	m_indexBuffer = std::make_unique<IndexBuffer>(const_cast<uint32_t*>(p_indices.data()), p_indices.size());

	int noSkinnedOffset = m_isSkinned == true ? 0 : 7;
	uint64_t vertexSize = sizeof(Vertex) - sizeof(float) * noSkinnedOffset;

	m_vertexArray.BindAttribute(0, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, 0);
	m_vertexArray.BindAttribute(1, *m_vertexBuffer, EType::FLOAT, 2, vertexSize, sizeof(float) * 3);
	m_vertexArray.BindAttribute(2, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, sizeof(float) * 5);
	m_vertexArray.BindAttribute(3, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, sizeof(float) * 8);
	m_vertexArray.BindAttribute(4, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, sizeof(float) * 11);

	if(m_isSkinned)
	{
		m_vertexArray.BindAttribute(5, *m_vertexBuffer, EType::INT, 4, vertexSize, sizeof(float) * 14);
		m_vertexArray.BindAttribute(6, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, sizeof(float) * 18);
	}
}

void LitchiRuntime::Mesh::ComputeBoundingSphere(const std::vector<Vertex>& p_vertices)
{
	m_boundingSphere.position = glm::vec3(0);
	m_boundingSphere.radius = 0.0f;

	if (!p_vertices.empty())
	{
		float minX = std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();
		float minZ = std::numeric_limits<float>::max();

		float maxX = std::numeric_limits<float>::min();
		float maxY = std::numeric_limits<float>::min();
		float maxZ = std::numeric_limits<float>::min();

		for (const auto& vertex : p_vertices)
		{
			minX = std::min(minX, vertex.position[0]);
			minY = std::min(minY, vertex.position[1]);
			minZ = std::min(minZ, vertex.position[2]);

			maxX = std::max(maxX, vertex.position[0]);
			maxY = std::max(maxY, vertex.position[1]);
			maxZ = std::max(maxZ, vertex.position[2]);
		}

		m_boundingSphere.position = glm::vec3{ minX + maxX, minY + maxY, minZ + maxZ } / 2.0f;

		for (const auto& vertex : p_vertices)
		{
			const auto& position = reinterpret_cast<const glm::vec3&>(vertex.position);
			m_boundingSphere.radius = std::max(m_boundingSphere.radius, glm::distance(m_boundingSphere.position, position));
		}
	}
}
