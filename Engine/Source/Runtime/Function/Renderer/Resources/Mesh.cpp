//
//#include <algorithm>
//
//#include "Mesh.h"
//
//LitchiRuntime::Mesh::Mesh(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex, const bool isSkinned) :
//	m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
//	m_indicesCount(static_cast<uint32_t>(p_indices.size())),
//	m_materialIndex(p_materialIndex),
//	m_isSkinned(isSkinned)
//{
//	vertices = p_vertices;
//	indices = p_indices;
//	CreateBuffers(vertices, indices);
//	ComputeBoundingSphere(p_vertices);
//}
//
//void LitchiRuntime::Mesh::Bind()
//{
//	m_vertexArray.Bind();
//}
//
//void LitchiRuntime::Mesh::Unbind()
//{
//	m_vertexArray.Unbind();
//}
//
//uint32_t LitchiRuntime::Mesh::GetVertexCount()
//{
//	return m_vertexCount;
//}
//
//uint32_t LitchiRuntime::Mesh::GetIndexCount()
//{
//	return m_indicesCount;
//}
//
//uint32_t LitchiRuntime::Mesh::GetMaterialIndex() const
//{
//	return m_materialIndex;
//}
//
//const LitchiRuntime::BoundingSphere& LitchiRuntime::Mesh::GetBoundingSphere() const
//{
//	return m_boundingSphere;
//}
//
//void LitchiRuntime::Mesh::CreateBuffers(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices)
//{
//	std::vector<Vertex> vertexData;
//	for (auto& vertex : p_vertices)
//	{
//		vertexData.push_back(vertex);
//	}
//	m_vertexBuffer = std::make_unique<VertexBuffer<Vertex>>(vertexData);
//	m_indexBuffer = std::make_unique<IndexBuffer>(const_cast<uint32_t*>(p_indices.data()), p_indices.size());
//
//	/*int noSkinnedOffset = m_isSkinned == true ? 0 : 7;
//	uint64_t vertexSize = sizeof(Vertex) - sizeof(float) * noSkinnedOffset;*/
//	uint64_t vertexSize = sizeof(Vertex);
//
//	m_vertexArray.BindAttribute(0, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, offsetof(Vertex, position));
//	m_vertexArray.BindAttribute(1, *m_vertexBuffer, EType::FLOAT, 2, vertexSize, offsetof(Vertex, texCoords));
//	m_vertexArray.BindAttribute(2, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, offsetof(Vertex, normals));
//	m_vertexArray.BindAttribute(3, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, offsetof(Vertex, tangent));
//	m_vertexArray.BindAttribute(4, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, offsetof(Vertex, bitangent));
//
//	if (m_isSkinned)
//	{
//		m_vertexArray.BindAttributeInteger(5, *m_vertexBuffer, EType::INT, 4, vertexSize, offsetof(Vertex, boneIndices));
//		m_vertexArray.BindAttribute(6, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, offsetof(Vertex, boneWeights));
//	}
//
//}
//
//void LitchiRuntime::Mesh::ComputeBoundingSphere(const std::vector<Vertex>& p_vertices)
//{
//	m_boundingSphere.position = glm::vec3(0);
//	m_boundingSphere.radius = 0.0f;
//
//	if (!p_vertices.empty())
//	{
//		float minX = std::numeric_limits<float>::max();
//		float minY = std::numeric_limits<float>::max();
//		float minZ = std::numeric_limits<float>::max();
//
//		float maxX = std::numeric_limits<float>::min();
//		float maxY = std::numeric_limits<float>::min();
//		float maxZ = std::numeric_limits<float>::min();
//
//		for (const auto& vertex : p_vertices)
//		{
//			minX = std::min(minX, vertex.position[0]);
//			minY = std::min(minY, vertex.position[1]);
//			minZ = std::min(minZ, vertex.position[2]);
//
//			maxX = std::max(maxX, vertex.position[0]);
//			maxY = std::max(maxY, vertex.position[1]);
//			maxZ = std::max(maxZ, vertex.position[2]);
//		}
//
//		m_boundingSphere.position = glm::vec3{ minX + maxX, minY + maxY, minZ + maxZ } / 2.0f;
//
//		for (const auto& vertex : p_vertices)
//		{
//			const auto& position = reinterpret_cast<const glm::vec3&>(vertex.position);
//			m_boundingSphere.radius = std::max(m_boundingSphere.radius, glm::distance(m_boundingSphere.position, position));
//		}
//	}
//}
//
//
//LitchiRuntime::UIMesh::UIMesh(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex):
//	m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
//	m_indicesCount(static_cast<uint32_t>(p_indices.size())),
//	m_materialIndex(p_materialIndex)
//{
//	vertices = p_vertices;
//	indices = p_indices;
//	CreateBuffers(vertices, indices);
//}
//
//void LitchiRuntime::UIMesh::Bind()
//{
//	m_vertexArray.Bind();
//}
//
//void LitchiRuntime::UIMesh::Unbind()
//{
//	m_vertexArray.Unbind();
//}
//
//uint32_t LitchiRuntime::UIMesh::GetVertexCount()
//{
//	return m_vertexCount;
//}
//
//uint32_t LitchiRuntime::UIMesh::GetIndexCount()
//{
//	return m_indicesCount;
//}
//
//void LitchiRuntime::UIMesh::CreateBuffers(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices)
//{
//	std::vector<Vertex> vertexData;
//	for (auto& vertex : p_vertices)
//	{
//		vertexData.push_back(vertex);
//	}
//	m_vertexBuffer = std::make_unique<VertexBuffer<Vertex>>(vertexData);
//	m_indexBuffer = std::make_unique<IndexBuffer>(const_cast<uint32_t*>(p_indices.data()), p_indices.size());
//
//	// 设置顶点数据 
//	uint64_t vertexSize = sizeof(Vertex);
//	m_vertexArray.BindAttribute(0, *m_vertexBuffer, EType::FLOAT, 3, vertexSize, offsetof(Vertex, position));
//	m_vertexArray.BindAttribute(1, *m_vertexBuffer, EType::FLOAT, 4, vertexSize, offsetof(Vertex, color));
//	m_vertexArray.BindAttribute(2, *m_vertexBuffer, EType::FLOAT, 2, vertexSize, offsetof(Vertex, texCoords));
//}
//
