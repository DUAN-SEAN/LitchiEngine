
#pragma once

#include <string>

#include "Runtime/Core/Math/BoundingBox.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering/Mesh.h"

using std::string;

namespace LitchiRuntime
{
    class MeshFilter :public Component {
    public:
        MeshFilter();
        ~MeshFilter();
    public:
       
    public:

        void PostResourceLoaded() override;
        void PostResourceModify() override;

        RTTR_ENABLE(Component)


    public:
        // Geometry/Mesh
        void SetGeometry(
            Mesh* mesh,
            int subMeshIndex = 0,
            const BoundingBox aabb = BoundingBox::Undefined
        );

        void GetGeometry(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PosTexNorTan>* vertices) const;

        // Properties
        int GetSubMeshIndex() { return m_sub_mesh_index; }
        void SetSubMeshIndex(int subMeshIndex) { m_sub_mesh_index = subMeshIndex; }
        std::string GetMeshPath() { return m_mesh_Path; }
        void SetMeshPath(std::string meshPath) { m_mesh_Path = meshPath; }
        uint32_t GetIndexOffset()                 const { return m_sub_mesh.m_geometry_index_offset; }
        uint32_t GetIndexCount()                  const { return m_sub_mesh.m_geometry_index_count; }
        uint32_t GetVertexOffset()                const { return m_sub_mesh.m_geometry_vertex_offset; }
        uint32_t GetVertexCount()                 const { return m_sub_mesh.m_geometry_vertex_count; }
        SubMesh GetSubMesh()                           const { return m_sub_mesh; }
        Mesh* GetMesh()                           const { return m_mesh; }
        const BoundingBox& GetBoundingBox() const { return m_bounding_box; }
        const BoundingBox& GetAAbb();

    private:
        
        SubMesh m_sub_mesh;
        int m_sub_mesh_index;

        /**
         * \brief 模型路径
         */
        std::string m_mesh_Path;

        Mesh* m_mesh = nullptr;
        BoundingBox m_bounding_box;
        BoundingBox m_bounding_box_transformed;

        // misc
        Matrix m_last_transform = Matrix::Identity;
    };
}
