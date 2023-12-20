

//= INCLUDES ================================
#include "Runtime/Core/pch.h"
#include "Mesh.h"
#include "Renderer.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_IndexBuffer.h"
#include "../RHI/RHI_Texture2D.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/Import/ModelImporter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
SP_WARNINGS_OFF
#include "meshoptimizer/meshoptimizer.h"
SP_WARNINGS_ON
//===========================================

//= NAMESPACES ================
using namespace std;
using namespace LitchiRuntime::Math;
//=============================

namespace LitchiRuntime
{
    Mesh::Mesh() : IResource(ResourceType::Mesh)
    {
        m_flags = GetDefaultFlags();
        m_model_scene = ApplicationBase::Instance()->sceneManager->CreateScene("Model Scene");
    }

    Mesh::~Mesh()
    {
        m_index_buffer  = nullptr;
        m_vertex_buffer = nullptr;
        ApplicationBase::Instance()->sceneManager->DestroyScene(m_model_scene);
    }

    void Mesh::Clear()
    {
        m_indices.clear();
        m_indices.shrink_to_fit();

        if(m_model_is_animation)
        {
            m_verticesWithBone.clear();
            m_verticesWithBone.shrink_to_fit();
        }
        else
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

        DEBUG_LOG_INFO("Loading {} took %d ms", FileSystem::GetFileNameFromFilePath(file_path).c_str(), static_cast<int>(timer.GetElapsedTimeMs()));

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
        size += uint32_t(m_indices.size()  * sizeof(uint32_t));

        if(m_model_is_animation)
        {
            size += uint32_t(m_verticesWithBone.size() * sizeof(RHI_Vertex_PosTexNorTanBone));
        }else
        {
            size += uint32_t(m_vertices.size() * sizeof(RHI_Vertex_PosTexNorTan));
        }
        
        return size;
    }

    void Mesh::GetGeometry(uint32_t index_offset, uint32_t index_count, uint32_t vertex_offset, uint32_t vertex_count, vector<uint32_t>* indices, vector<RHI_Vertex_PosTexNorTan>* vertices)
    {
        SP_ASSERT_MSG(indices != nullptr || vertices != nullptr, "Indices and vertices vectors can't both be null");

        if (indices)
        {
            SP_ASSERT_MSG(index_count != 0, "Index count can't be 0");

            const auto index_first = m_indices.begin() + index_offset;
            const auto index_last  = m_indices.begin() + index_offset + index_count;
            *indices               = vector<uint32_t>(index_first, index_last);
        }

        if (vertices)
        {
            SP_ASSERT_MSG(vertex_count != 0, "Index count can't be 0");

            const auto vertex_first = m_vertices.begin() + vertex_offset;
            const auto vertex_last  = m_vertices.begin() + vertex_offset + vertex_count;
            *vertices               = vector<RHI_Vertex_PosTexNorTan>(vertex_first, vertex_last);
        }
    }

	void Mesh::GetGeometry(uint32_t index_offset, uint32_t index_count, uint32_t vertex_offset, uint32_t vertex_count, vector<uint32_t>* indices, vector<RHI_Vertex_PosTexNorTanBone>* vertices)
    {
        SP_ASSERT_MSG(indices != nullptr || vertices != nullptr, "Indices and vertices vectors can't both be null");

        if (indices)
        {
            SP_ASSERT_MSG(index_count != 0, "Index count can't be 0");

            const auto index_first = m_indices.begin() + index_offset;
            const auto index_last = m_indices.begin() + index_offset + index_count;
            *indices = vector<uint32_t>(index_first, index_last);
        }

        if (vertices)
        {
            SP_ASSERT_MSG(vertex_count != 0, "Index count can't be 0");

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
        if(m_model_is_animation)
        {
            return static_cast<uint32_t>(m_verticesWithBone.size());
        }else
        {
            return static_cast<uint32_t>(m_vertices.size());
        }
    }

    uint32_t Mesh::GetIndexCount() const
    {
        return static_cast<uint32_t>(m_indices.size());
    }

    void Mesh::ComputeAabb()
    {
        if (m_model_is_animation)
        {
            SP_ASSERT_MSG(m_verticesWithBone.size() != 0, "There are no vertices");
            m_aabb = BoundingBox(m_verticesWithBone.data(), static_cast<uint32_t>(m_verticesWithBone.size()));
        }else
        {
            SP_ASSERT_MSG(m_vertices.size() != 0, "There are no vertices");
            m_aabb = BoundingBox(m_vertices.data(), static_cast<uint32_t>(m_vertices.size()));
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
        float scale_offset     = m_aabb.GetExtents().Length();
        float normalized_scale = 1.0f / scale_offset;

        return normalized_scale;
    }
    
    void Mesh::Optimize()
    {
        SP_ASSERT(!m_indices.empty());
        SP_ASSERT(!m_vertices.empty());

        uint32_t index_count                     = static_cast<uint32_t>(m_indices.size());
        uint32_t vertex_count                    = static_cast<uint32_t>(m_vertices.size());
        size_t vertex_size                       = sizeof(RHI_Vertex_PosTexNorTan);
        vector<uint32_t> indices                 = m_indices;
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
        SP_ASSERT_MSG(!m_indices.empty(), "There are no indices");
        m_index_buffer = make_shared<RHI_IndexBuffer>(false, (string("mesh_index_buffer_") + m_object_name).c_str());
        m_index_buffer->Create(m_indices);

        if(m_model_is_animation)
        {
            SP_ASSERT_MSG(!m_verticesWithBone.empty(), "There are no vertices");
            m_vertex_buffer = make_shared<RHI_VertexBuffer>(false, (string("mesh_vertex_buffer_") + m_object_name).c_str());
            m_vertex_buffer->Create(m_verticesWithBone);
        }else
        {
            SP_ASSERT_MSG(!m_vertices.empty(), "There are no vertices");
            m_vertex_buffer = make_shared<RHI_VertexBuffer>(false, (string("mesh_vertex_buffer_") + m_object_name).c_str());
            m_vertex_buffer->Create(m_vertices);
        }
    }

    void Mesh::AddMaterial(Material* material, GameObject* entity) const
    {
        SP_ASSERT(material != nullptr);
        SP_ASSERT(entity != nullptr);

        // Create a file path for this material (required for the material to be able to be cached by the resource cache)
        const string spartan_asset_path = FileSystem::GetDirectoryFromFilePath(GetResourceFilePathNative()) + material->GetObjectName() + EXTENSION_MATERIAL;
        material->SetResourceFilePath(spartan_asset_path);

        if(!m_model_is_animation)
        {
            // Create a Renderable and pass the material to it
            entity->AddComponent<MeshRenderer>()->SetMaterial(material);
        }else
        {
            // Create a Renderable and pass the material to it
            entity->AddComponent<SkinnedMeshRenderer>()->SetMaterial(material);
        }
    }

    void Mesh::AddTexture(Material* material, const MaterialTexture texture_type, const string& file_path, bool is_gltf)
    {
        SP_ASSERT(material != nullptr);
        SP_ASSERT(!file_path.empty());

        // Try to get the texture
        const auto tex_name = FileSystem::GetFileNameWithoutExtensionFromFilePath(file_path);
        shared_ptr<RHI_Texture> texture = ResourceCache::GetByName<RHI_Texture2D>(tex_name);

        // todo:
        //if (texture)
        //{
        //    material->SetTexture(texture_type, texture);
        //}
        //else // If we didn't get a texture, it's not cached, hence we have to load it and cache it now
        //{
        //    // Load texture
        //    texture = ResourceCache::Load<RHI_Texture2D>(file_path, RHI_Texture_Srv | RHI_Texture_Mips | RHI_Texture_PerMipViews | RHI_Texture_Compressed);

        //    // Set the texture to the provided material
        //    material->SetTexture(texture_type, texture);
        //}
    }

    void Mesh::AddSubMesh(SubMesh subMesh,int& subMeshIndex)
    {
        subMeshIndex = m_subMeshArr.size();
        m_subMeshArr.push_back(subMesh);
    }

    SubMesh Mesh::GetSubMesh(int index,bool& result)
    {
        if(index>=m_subMeshArr.size() || index<0)
        {
            result = false;
            return SubMesh();
        }

        result = true;
        return m_subMeshArr[index];
    }
}
