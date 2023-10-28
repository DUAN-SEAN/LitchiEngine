
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

    	/**
         * \brief 模型路径
         */
        std::string modelPath;

        int meshIndex;

        RTTR_ENABLE(Component)


    public:
        // Geometry/Mesh
        void SetGeometry(
            Mesh* mesh,
            const BoundingBox aabb = BoundingBox::Undefined,
            uint32_t index_offset = 0, uint32_t index_count = 0,
            uint32_t vertex_offset = 0, uint32_t vertex_count = 0
        );
        void GetGeometry(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PosTexNorTan>* vertices) const;

        // Properties
        uint32_t GetIndexOffset()                 const { return m_geometry_index_offset; }
        uint32_t GetIndexCount()                  const { return m_geometry_index_count; }
        uint32_t GetVertexOffset()                const { return m_geometry_vertex_offset; }
        uint32_t GetVertexCount()                 const { return m_geometry_vertex_count; }
        Mesh* GetMesh()                           const { return m_mesh; }
        const BoundingBox& GetBoundingBox() const { return m_bounding_box; }
        const BoundingBox& GetAabb();

    private:

        // geometry/mesh
        uint32_t m_geometry_index_offset = 0;
        uint32_t m_geometry_index_count = 0;
        uint32_t m_geometry_vertex_offset = 0;
        uint32_t m_geometry_vertex_count = 0;
        Mesh* m_mesh = nullptr;
        BoundingBox m_bounding_box;
        BoundingBox m_bounding_box_transformed;

        // misc
        Matrix m_last_transform = Matrix::Identity;
    };
}
