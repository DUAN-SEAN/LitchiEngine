
#pragma once

//= INCLUDES =============================
#include <vector>
#include <memory>
#include "../RHI/RHI_Definitions.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Definitions.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
//========================================

namespace LitchiRuntime
{
    class LC_CLASS SphereGeometry
    {
    public:
        SphereGeometry();
        ~SphereGeometry() = default;
       
        const auto& GetVertexBuffer()   const { return m_vertex_buffer; }
        const auto& GetIndexBuffer()   const { return m_index_buffer; }
        const uint32_t GetVertexCount() const { return m_vertex_count; }
        const uint32_t GetIndexCount() const { return m_index_count; }
        const Matrix& GetWorldMatrix() { return m_worldMatrix; }

    private:
        uint32_t m_vertex_count   = 0;
        uint32_t m_index_count   = 0;

        Matrix m_worldMatrix{Matrix::Identity};

        std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;
        std::shared_ptr<RHI_IndexBuffer> m_index_buffer;
    };
}
