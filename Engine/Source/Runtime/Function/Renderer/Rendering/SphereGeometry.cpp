
//= INCLUDES =============================
#include "Runtime/Core/pch.h"
#include "SphereGeometry.h"

#include "Geometry.h"
#include "../RHI/RHI_Vertex.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_IndexBuffer.h"
//========================================

//= NAMESPACES ================
using namespace std;
using namespace LitchiRuntime::Math;
//=============================

namespace LitchiRuntime
{
    SphereGeometry::SphereGeometry()
    {
        // create vertices

        std::vector<RHI_Vertex_PosTexNorTan> vertices{};
        std::vector<uint32_t> indices{};
        Geometry::CreateSphere(&vertices, &indices);

        m_vertex_count = static_cast<uint32_t>(vertices.size());
        m_index_count = static_cast<uint32_t>(indices.size());

        // create vertex buffer
        m_vertex_buffer = make_shared<RHI_VertexBuffer>(false, "SphereGeometry");
        m_vertex_buffer->Create(vertices);

        m_index_buffer = make_shared<RHI_IndexBuffer>(false, "SphereGeometry");
        m_index_buffer->Create(indices);


        m_worldMatrix = Matrix::CreateScale(1) * Matrix::CreateTranslation(Vector3::Zero) * Matrix::CreateRotation(Quaternion::Identity);
    }
}
