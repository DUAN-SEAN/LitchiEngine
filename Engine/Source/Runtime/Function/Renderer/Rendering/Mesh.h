/*
Copyright(c) 2016-2023 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES =====================
#include <vector>
#include "Material.h"
#include "../Resource/IResource.h"
#include "Runtime/Core/Math/BoundingBox.h"
#include "../RHI/RHI_Vertex.h"
//================================

namespace LitchiRuntime
{
    enum class MeshFlags : uint32_t
    {
        ImportRemoveRedundantData = 1 << 0,
        ImportLights              = 1 << 1,
        ImportCombineMeshes       = 1 << 2,
        ImportNormalizeScale      = 1 << 3,
        OptimizeVertexCache       = 1 << 4,
        OptimizeVertexFetch       = 1 << 5,
        OptimizeOverdraw          = 1 << 6,
    };

    class Mesh : public IResource
    {
    public:
        Mesh();
        ~Mesh();

        // IResource
        bool LoadFromFile(const std::string& file_path) override;
        bool SaveToFile(const std::string& file_path) override;

        // Geometry
        void Clear();
        void GetGeometry(
            uint32_t indexOffset,
            uint32_t indexCount,
            uint32_t vertexOffset,
            uint32_t vertexCount,
            std::vector<uint32_t>* indices,
            std::vector<RHI_Vertex_PosTexNorTan>* vertices
        );
        uint32_t GetMemoryUsage() const;

        // Add geometry
        void AddVertices(const std::vector<RHI_Vertex_PosTexNorTan>& vertices, uint32_t* vertex_offset_out = nullptr);
        void AddIndices(const std::vector<uint32_t>& indices, uint32_t* index_offset_out = nullptr);

        // Get geometry
        std::vector<RHI_Vertex_PosTexNorTan>& GetVertices() { return m_vertices; }
        std::vector<uint32_t>& GetIndices()                 { return m_indices; }

        // Get counts
        uint32_t GetVertexCount() const;
        uint32_t GetIndexCount() const;

        // AABB
        const BoundingBox& GetAabb() const { return m_aabb; }
        void ComputeAabb();

        // GPU buffers
        void CreateGpuBuffers();
        RHI_IndexBuffer* GetIndexBuffer()   { return m_index_buffer.get();  }
        RHI_VertexBuffer* GetVertexBuffer() { return m_vertex_buffer.get(); }

        // Root entity
        Entity* GetRootEntity() { return m_root_entity.lock().get(); }
        void SetRootEntity(std::shared_ptr<Entity>& entity) { m_root_entity = entity; }

        // Misc
        uint32_t GetFlags() const { return m_flags; }
        static uint32_t GetDefaultFlags();
        float ComputeNormalizedScale();
        void Optimize();
        void AddMaterial(std::shared_ptr<Material>& material, Entity* entity) const;
        void AddTexture(std::shared_ptr<Material>& material, MaterialTexture texture_type, const std::string& file_path, bool is_gltf);

    private:
        // Geometry
        std::vector<RHI_Vertex_PosTexNorTan> m_vertices;
        std::vector<uint32_t> m_indices;

        // GPU buffers
        std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;
        std::shared_ptr<RHI_IndexBuffer> m_index_buffer;

        // AABB
        BoundingBox m_aabb;

        // Sync primitives
        std::mutex m_mutex_indices;
        std::mutex m_mutex_vertices;

        // Misc
        std::weak_ptr<Entity> m_root_entity;
    };
}
