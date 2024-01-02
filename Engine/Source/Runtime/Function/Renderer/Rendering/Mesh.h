
#pragma once

//= INCLUDES =====================
#include <vector>

#include "Animation.h"
#include "Material.h"
#include "../Resource/IResource.h"
#include "Runtime/Core/Math/BoundingBox.h"
#include "../RHI/RHI_Vertex.h"
#include "Runtime/Function/Prefab/Prefab.h"
#include "Runtime/Function/Scene/SceneManager.h"
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

    struct SubMesh
    {
        uint32_t m_geometry_index_offset = 0;
        uint32_t m_geometry_index_count = 0;
        uint32_t m_geometry_vertex_offset = 0;
        uint32_t m_geometry_vertex_count = 0;
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
        void GetGeometry(
            uint32_t indexOffset,
            uint32_t indexCount,
            uint32_t vertexOffset,
            uint32_t vertexCount,
            std::vector<uint32_t>* indices,
            std::vector<RHI_Vertex_PosTexNorTanBone>* vertices
        );
        uint32_t GetMemoryUsage() const;

        uint32_t IsAnimationModel() const { return m_model_is_animation; }
        void SetIsAnimationModel(bool model_is_animation) { m_model_is_animation = model_is_animation; }

        // Add geometry
        void AddVertices(const std::vector<RHI_Vertex_PosTexNorTan>& vertices, uint32_t* vertex_offset_out = nullptr);
        void AddIndices(const std::vector<uint32_t>& indices, uint32_t* index_offset_out = nullptr);


        void AddVertices(const std::vector<RHI_Vertex_PosTexNorTanBone>& vertices, uint32_t* vertex_offset_out = nullptr);

        // Get geometry
        std::vector<RHI_Vertex_PosTexNorTan>& GetVertices() { return m_vertices; }
        std::vector<RHI_Vertex_PosTexNorTanBone>& GetVerticesWithBone() { return m_verticesWithBone; }
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
        Prefab* GetModelScene() { return m_model_scene; }
        GameObject* GetRootEntity() { return m_root_entity; }
        void SetRootEntity(GameObject* entity) { m_root_entity = entity; }

        // SubMesh
        void AddSubMesh(SubMesh subMesh, int& subMeshIndex);
        SubMesh GetSubMesh(int index, bool& result);

        // Bone
        std::vector<BoneInfo>& GetBones() { return m_boneInfoArr; }
        std::unordered_map<std::string, uint32_t>& GetBoneMap() { return m_boneMap; }
        std::unordered_map<std::string, AnimationClip>& GetAnimationClipMap() { return m_animationClipMap; }

        // Misc
        uint32_t GetFlags() const { return m_flags; }
        static uint32_t GetDefaultFlags();
        float ComputeNormalizedScale();
        void Optimize();
        void AddMaterial(Material* material, GameObject* entity) const;
        void AddTexture(Material* material, MaterialTexture texture_type, const std::string& file_path, bool is_gltf);
       
    	void GetBoneMapping(std::unordered_map<std::string, uint32_t>& boneMapping) {
            boneMapping = this->m_boneMap;
        }
        void GetBoneOffsets(std::vector<Matrix>& boneOffsets) {
            for (size_t i = 0; i < m_boneInfoArr.size(); i++)
                boneOffsets.push_back(m_boneInfoArr[i].boneOffset);
        }
        void GetNodeOffsets(std::vector<Matrix>& nodeOffsets) {
            for (size_t i = 0; i < m_boneInfoArr.size(); i++)
                nodeOffsets.push_back(m_boneInfoArr[i].defaultOffset);
        }
        void GetBoneHierarchy(std::vector<int>& boneHierarchy) {
            for (size_t i = 0; i < m_boneInfoArr.size(); i++)
                boneHierarchy.push_back(m_boneInfoArr[i].parentIndex);
        }
        void GetAnimations(std::unordered_map<std::string, AnimationClip>& animations) {
            animations.insert(this->m_animationClipMap.begin(), this->m_animationClipMap.end());
        }

    private:

        bool m_model_is_animation = false;

        // Geometry
        std::vector<RHI_Vertex_PosTexNorTan> m_vertices;
        std::vector<RHI_Vertex_PosTexNorTanBone> m_verticesWithBone;
        std::vector<uint32_t> m_indices;

        // GPU buffers
        std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;
        std::shared_ptr<RHI_IndexBuffer> m_index_buffer;

        // AABB
        BoundingBox m_aabb;

        // sub Mesh collection
        std::vector<SubMesh> m_subMeshArr;


        //Bone/Animation Information
        std::vector<BoneInfo> m_boneInfoArr;
        std::unordered_map<std::string, uint32_t> m_boneMap;
        std::unordered_map<std::string, AnimationClip> m_animationClipMap;

        // Sync primitives
        std::mutex m_mutex_indices;
        std::mutex m_mutex_vertices;

        // Misc Model GameObject in Model Scene
        Prefab* m_model_scene;
        GameObject* m_root_entity;
    };
}
