

//= INCLUDES ================================
#include "Runtime/Core/pch.h"
#include "Mesh.h"
#include "Renderer.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_IndexBuffer.h"
#include "../RHI/RHI_Texture2D.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/Import/ModelImporter.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
LC_WARNINGS_OFF
#include "meshoptimizer/meshoptimizer.h"
LC_WARNINGS_ON
//===========================================

//= NAMESPACES ================
using namespace std;
using namespace LitchiRuntime::Math;
//=============================

namespace LitchiRuntime
{
	Mesh::Mesh() : IResource(ResourceType::Mesh), m_model_prefab(nullptr)
	{
		m_flags = GetDefaultFlags();
	}

	Mesh::~Mesh()
	{
		m_index_buffer = nullptr;
		m_vertex_buffer = nullptr;
		if (m_model_prefab)
		{
			ApplicationBase::Instance()->prefabManager->UnloadResource(m_model_prefab->GetResourceFilePathAsset());
		}
	}

	void Mesh::Clear()
	{
		m_indices.clear();
		m_indices.shrink_to_fit();

		// if (m_model_is_animation)
		{
			m_verticesWithBone.clear();
			m_verticesWithBone.shrink_to_fit();
		}
		// else
		{
			m_vertices.clear();
			m_vertices.shrink_to_fit();
		}
	}

	bool Mesh::LoadFromFile(const string& file_path)
	{
		const Stopwatch timer;

		if (file_path.empty() || FileSystem::IsDirectory(file_path))
		{
			DEBUG_LOG_WARN("Invalid file path");
			return false;
		}

		if (m_model_prefab)
		{
			// ApplicationBase::Instance()->prefabManager->UnloadResource(m_model_prefab->GetResourceFilePathAsset());
			m_model_prefab = nullptr;
		}
		m_model_prefab = ApplicationBase::Instance()->prefabManager->CreatePrefab(FileSystem::GetRelativePathAssetFromNative(file_path));

		//// load engine format todo:
		//if (FileSystem::GetExtensionFromFilePath(file_path) == EXTENSION_MODEL)
		//{
		//    // deserialize
		//    auto file = make_unique<FileStream>(file_path, FileStream_Read);
		//    if (!file->IsOpen())
		//        return false;

		//    SetResourceFilePath(file->ReadAs<string>());
		//    file->Read(&m_indices);
		//    file->Read(&m_vertices);

		//    //Optimize();
		//    ComputeAabb();
		//    ComputeNormalizedScale();
		//    CreateGpuBuffers();
		//}
		//// load foreign format
		//else
		{
			SetResourceFilePath(file_path);

			if (!ModelImporter::Load(this, file_path))
				return false;
		}

		// compute memory usage
		{
			// cpu
			m_object_size_cpu = GetMemoryUsage();

			// gpu
			if (m_vertex_buffer && m_index_buffer)
			{
				m_object_size_gpu = m_vertex_buffer->GetObjectSizeGpu();
				m_object_size_gpu += m_index_buffer->GetObjectSizeGpu();
			}
		}

		DEBUG_LOG_INFO("Loading {} took {} ms", FileSystem::GetFileNameFromFilePath(file_path).c_str(), static_cast<int>(timer.GetElapsedTimeMs()));

		return true;
	}

	bool Mesh::SaveToFile(const string& file_path)
	{
		// todo:
		/*auto file = make_unique<FileStream>(file_path, FileStream_Write);
		if (!file->IsOpen())
			return false;

		file->Write(GetResourceFilePath());
		file->Write(m_indices);
		file->Write(m_vertices);

		file->Close();*/

		return true;
	}

	uint32_t Mesh::GetMemoryUsage() const
	{
		uint32_t size = 0;
		size += uint32_t(m_indices.size() * sizeof(uint32_t));

		// if (m_model_is_animation)
		{
			size += uint32_t(m_verticesWithBone.size() * sizeof(RHI_Vertex_PosTexNorTanBone));
		}
		// else
		{
			size += uint32_t(m_vertices.size() * sizeof(RHI_Vertex_PosTexNorTan));
		}

		return size;
	}

	void Mesh::GetGeometry(uint32_t index_offset, uint32_t index_count, uint32_t vertex_offset, uint32_t vertex_count, vector<uint32_t>* indices, vector<RHI_Vertex_PosTexNorTan>* vertices)
	{
		LC_ASSERT_MSG(indices != nullptr || vertices != nullptr, "Indices and vertices vectors can't both be null");

		if (indices)
		{
			LC_ASSERT_MSG(index_count != 0, "Index count can't be 0");

			const auto index_first = m_indices.begin() + index_offset;
			const auto index_last = m_indices.begin() + index_offset + index_count;
			*indices = vector<uint32_t>(index_first, index_last);
		}

		if (vertices)
		{
			LC_ASSERT_MSG(vertex_count != 0, "Index count can't be 0");

			const auto vertex_first = m_vertices.begin() + vertex_offset;
			const auto vertex_last = m_vertices.begin() + vertex_offset + vertex_count;
			*vertices = vector<RHI_Vertex_PosTexNorTan>(vertex_first, vertex_last);
		}
	}

	void Mesh::GetGeometry(uint32_t index_offset, uint32_t index_count, uint32_t vertex_offset, uint32_t vertex_count, vector<uint32_t>* indices, vector<RHI_Vertex_PosTexNorTanBone>* vertices)
	{
		LC_ASSERT_MSG(indices != nullptr || vertices != nullptr, "Indices and vertices vectors can't both be null");

		if (indices)
		{
			LC_ASSERT_MSG(index_count != 0, "Index count can't be 0");

			const auto index_first = m_indices.begin() + index_offset;
			const auto index_last = m_indices.begin() + index_offset + index_count;
			*indices = vector<uint32_t>(index_first, index_last);
		}

		if (vertices)
		{
			LC_ASSERT_MSG(vertex_count != 0, "Index count can't be 0");

			const auto vertex_first = m_verticesWithBone.begin() + vertex_offset;
			const auto vertex_last = m_verticesWithBone.begin() + vertex_offset + vertex_count;
			*vertices = vector<RHI_Vertex_PosTexNorTanBone>(vertex_first, vertex_last);
		}
	}

	void Mesh::AddVertices(const vector<RHI_Vertex_PosTexNorTan>& vertices, uint32_t* vertex_offset_out /*= nullptr*/)
	{
		lock_guard lock(m_mutex_vertices);

		if (vertex_offset_out)
		{
			*vertex_offset_out = static_cast<uint32_t>(m_vertices.size());
		}

		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	}

	void Mesh::AddVertices(const std::vector<RHI_Vertex_PosTexNorTanBone>& vertices, uint32_t* vertex_offset_out /*= nullptr*/)
	{
		lock_guard lock(m_mutex_vertices);

		if (vertex_offset_out)
		{
			*vertex_offset_out = static_cast<uint32_t>(m_verticesWithBone.size());
		}

		m_verticesWithBone.insert(m_verticesWithBone.end(), vertices.begin(), vertices.end());
	}

	void Mesh::AddIndices(const vector<uint32_t>& indices, uint32_t* index_offset_out /*= nullptr*/)
	{
		lock_guard lock(m_mutex_vertices);

		if (index_offset_out)
		{
			*index_offset_out = static_cast<uint32_t>(m_indices.size());
		}

		m_indices.insert(m_indices.end(), indices.begin(), indices.end());
	}

	uint32_t Mesh::GetVertexCount() const
	{
		uint32_t size = 0;
		// if (m_model_is_animation)
		{
			size +=  static_cast<uint32_t>(m_verticesWithBone.size());
		}
		// else
		{
			size += static_cast<uint32_t>(m_vertices.size());
		}

		return size;
	}

	uint32_t Mesh::GetIndexCount() const
	{
		return static_cast<uint32_t>(m_indices.size());
	}

	void Mesh::ComputeAabb()
	{
		if(m_verticesWithBone.size()> 0)
		{
			m_aabbWithBone = BoundingBox(m_verticesWithBone.data(), static_cast<uint32_t>(m_verticesWithBone.size()));
		}

		if(m_vertices.size() > 0)
		{
			m_aabb = BoundingBox(m_vertices.data(), static_cast<uint32_t>(m_vertices.size()));
		}

		// compute sub mesh aabb
		for (int subMeshIndex = 0; subMeshIndex < m_subMeshArr.size(); subMeshIndex++)
		{
			auto& subMesh = m_subMeshArr[subMeshIndex];
			if (subMesh.m_isSkinnedMesh)
			{
				m_subMesh_aabb_arr.push_back(BoundingBox(m_verticesWithBone.data(), subMesh.m_geometryVertexCount, subMesh.m_geometryVertexOffset));
			}else
			{
				m_subMesh_aabb_arr.push_back(BoundingBox(m_vertices.data(), subMesh.m_geometryVertexCount, subMesh.m_geometryVertexOffset));
			}
		}
	}

	uint32_t Mesh::GetDefaultFlags()
	{
		return
			static_cast<uint32_t>(MeshFlags::ImportRemoveRedundantData) |
			static_cast<uint32_t>(MeshFlags::ImportNormalizeScale);
		//static_cast<uint32_t>(MeshFlags::OptimizeVertexCache) |
		//static_cast<uint32_t>(MeshFlags::OptimizeOverdraw) |
		//static_cast<uint32_t>(MeshFlags::OptimizeVertexFetch);
	}

	float Mesh::ComputeNormalizedScale()
	{
		float scale_offset = m_aabb.GetExtents().Length();
		float normalized_scale = 1.0f / scale_offset;

		return normalized_scale;
	}

	void Mesh::Optimize()
	{
		LC_ASSERT(!m_indices.empty());
		LC_ASSERT(!m_vertices.empty());

		uint32_t index_count = static_cast<uint32_t>(m_indices.size());
		uint32_t vertex_count = static_cast<uint32_t>(m_vertices.size());
		size_t vertex_size = sizeof(RHI_Vertex_PosTexNorTan);
		vector<uint32_t> indices = m_indices;
		vector<RHI_Vertex_PosTexNorTan> vertices = m_vertices;

		// vertex cache optimization
		// improves the GPU's post-transform cache hit rate, reducing the required vertex shader invocations
		if (m_flags & static_cast<uint32_t>(MeshFlags::OptimizeVertexCache))
		{
			meshopt_optimizeVertexCache(&indices[0], &m_indices[0], index_count, vertex_count);
		}

		// overdraw optimization
		// minimizes overdraw by reordering triangles, aiming to reduce pixel shader invocations
		if (m_flags & static_cast<uint32_t>(MeshFlags::OptimizeOverdraw))
		{
			meshopt_optimizeOverdraw(&indices[0], &indices[0], index_count, &m_vertices[0].pos[0], vertex_count, vertex_size, 1.05f);
		}

		// vertex fetch optimization
		// reorders vertices and changes indices to improve vertex fetch cache performance, reducing the bandwidth needed to fetch vertices
		if (m_flags & static_cast<uint32_t>(MeshFlags::OptimizeVertexFetch))
		{
			meshopt_optimizeVertexFetch(&m_vertices[0], &indices[0], index_count, &vertices[0], vertex_count, vertex_size);
		}

		// store the updated indices back to m_indices
		m_indices = indices;
	}

	void Mesh::CreateGpuBuffers()
	{
		LC_ASSERT_MSG(!m_indices.empty(), "There are no indices");
		m_index_buffer = make_shared<RHI_IndexBuffer>(false, (string("mesh_index_buffer_") + m_object_name).c_str());
		m_index_buffer->Create(m_indices);

		if (!m_verticesWithBone.empty())
		{
			LC_ASSERT_MSG(!m_verticesWithBone.empty(), "There are no vertices");
			m_vertex_bufferWithBone = make_shared<RHI_VertexBuffer>(false, (string("mesh_vertex_buffer_withBone") + m_object_name).c_str());
			m_vertex_bufferWithBone->Create(m_verticesWithBone);
		}

		if(!m_vertices.empty())
		{
			m_vertex_buffer = make_shared<RHI_VertexBuffer>(false, (string("mesh_vertex_buffer_withoutBone") + m_object_name).c_str());
			m_vertex_buffer->Create(m_vertices);
		}
	}

	void Mesh::AddMaterial(Material* material, GameObject* entity) const
	{
		LC_ASSERT(material != nullptr);
		LC_ASSERT(entity != nullptr);

		// Create a file path for this material (required for the material to be able to be cached by the resource cache)
		const string spartan_asset_path = FileSystem::GetDirectoryFromFilePath(GetResourceFilePathNative()) + material->GetObjectName() + EXTENSION_MATERIAL;
		material->SetResourceFilePath(spartan_asset_path);
		entity->GetComponent<MeshRenderer>()->SetMaterial(material);
	}

	void Mesh::AddTexture(Material* material, const MaterialTexture texture_type, const string& file_path, bool is_gltf)
	{
		LC_ASSERT(material != nullptr);
		LC_ASSERT(!file_path.empty());

		auto texture = ApplicationBase::Instance()->textureManager->LoadResource(file_path);
		if (!texture)
		{
			// load default texture
			auto texture = Renderer::GetStandardTexture(Renderer_StandardTexture::White);
		}
		material->SetTexture(texture_type, texture);
	}

	void Mesh::AddSubMesh(SubMesh subMesh, int& subMeshIndex)
	{
		subMeshIndex = m_subMeshArr.size();
		m_subMeshArr.push_back(subMesh);
	}

	SubMesh Mesh::GetSubMesh(int index, bool& result)
	{
		if (index >= m_subMeshArr.size() || index < 0)
		{
			result = false;
			return SubMesh();
		}

		result = true;
		return m_subMeshArr[index];
	}
}
