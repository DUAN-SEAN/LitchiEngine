
//= INCLUDES =============================

#include "Runtime/Core/pch.h"
#include "PlaneGeometry.h"

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
    PlaneGeometry::PlaneGeometry()
    {
        // create vertices

        std::vector<RHI_Vertex_PosTexNorTan> vertices{};
        std::vector<uint32_t> indices{};
        Geometry::CreateQuad(&vertices, &indices);

        m_vertex_count = static_cast<uint32_t>(vertices.size());
        m_index_count = static_cast<uint32_t>(indices.size());

        // create vertex buffer
        m_vertex_buffer = make_shared<RHI_VertexBuffer>(false, "PlaneGeometry");
        m_vertex_buffer->Create(vertices);

        m_index_buffer = make_shared<RHI_IndexBuffer>(false, "PlaneGeometry");
        m_index_buffer->Create(indices);


        m_worldMatrix = Matrix::CreateScale({10.0f,1.0f,10.0f}) * Matrix::CreateTranslation(Vector3::Zero) * Matrix::CreateRotation(Quaternion::FromEulerAngles(-90.0f,0.0f,0.0f));
    }
}
