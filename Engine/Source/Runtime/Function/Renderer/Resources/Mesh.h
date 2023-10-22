
#pragma once

#include <vector>
#include <memory>

#include "IMesh.h"
#include "Runtime/Function/Renderer/Buffers/IndexBuffer.h"
#include "Runtime/Function/Renderer/Buffers/VertexArray.h"
#include "Runtime/Function/Renderer/Buffers/VertexBuffer.h"
#include "Runtime/Function/Renderer/Geometry/BoundingSphere.h"
#include "Runtime/Function/Renderer/Geometry/Vertex.h"

namespace LitchiRuntime
{
	///**
	//* Standard mesh of Model
	//*/
	//class Mesh : public IMesh
	//{
	//public:
	//	/**
	//	* Create a mesh with the given vertices, indices and material index
	//	* @param p_vertices
	//	* @param p_indices
	//	* @param p_materialIndex
	//	*/
	//	Mesh(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex, const bool isSkinned = false);

	//	/**
	//	* Bind the mesh (Actually bind its VAO)
	//	*/
	//	virtual void Bind() override;

	//	/**
	//	* Unbind the mesh (Actually unbind its VAO)
	//	*/
	//	virtual void Unbind() override;

	//	/**
	//	* Returns the number of vertices
	//	*/
	//	virtual uint32_t GetVertexCount() override;

	//	/**
	//	* Returns the number of indices
	//	*/
	//	virtual uint32_t GetIndexCount() override;

	//	/**
	//	* Returns the material index of the mesh
	//	*/
	//	uint32_t GetMaterialIndex() const;

	//	/**
	//	* Returns the bounding sphere of the mesh
	//	*/
	//	const BoundingSphere& GetBoundingSphere() const;

	//	VertexBuffer<Vertex>* GetVertexBuffer() const
	//	{
	//		return m_vertexBuffer.get();
	//	}

	//	IndexBuffer* GetIndexBuffer() const
	//	{
	//		return m_indexBuffer.get();
	//	}

	//	std::vector<Vertex> vertices;

	//	std::vector<uint32_t> indices;

	//private:
	//	void CreateBuffers(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices);
	//	void ComputeBoundingSphere(const std::vector<Vertex>& p_vertices);

	//private:
	//	const uint32_t m_vertexCount;
	//	const uint32_t m_indicesCount;
	//	const uint32_t m_materialIndex;

	//	VertexArray							m_vertexArray;
	//	std::unique_ptr<VertexBuffer<Vertex>>	m_vertexBuffer;
	//	std::unique_ptr<IndexBuffer>			m_indexBuffer;

	//	BoundingSphere m_boundingSphere;

	//	bool m_isSkinned;

	//	// todo 新增Vertex, 缓存顶点信息
	//	// todo 新增Index, 缓存索引信息
	//};

	//class UIMesh :public IMesh
	//{
	//public:
	//	UIMesh(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex);

	//	/**
	//	* Bind the mesh (Actually bind its VAO)
	//	*/
	//	virtual void Bind() override;

	//	/**
	//	* Unbind the mesh (Actually unbind its VAO)
	//	*/
	//	virtual void Unbind() override;

	//	/**
	//	* Returns the number of vertices
	//	*/
	//	virtual uint32_t GetVertexCount() override;

	//	/**
	//	* Returns the number of indices
	//	*/
	//	virtual uint32_t GetIndexCount() override;

	//	std::vector<Vertex> vertices;

	//	std::vector<uint32_t> indices;

	//private:

	//	void CreateBuffers(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices);
	//private:
	//	const uint32_t m_vertexCount;
	//	const uint32_t m_indicesCount;
	//	const uint32_t m_materialIndex;

	//	VertexArray							m_vertexArray;
	//	std::unique_ptr<VertexBuffer<Vertex>>	m_vertexBuffer;
	//	std::unique_ptr<IndexBuffer>			m_indexBuffer;

	//	BoundingSphere m_boundingSphere;
	//};

}
