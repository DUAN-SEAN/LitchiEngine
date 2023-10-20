
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
    class SP_CLASS Grid
    {
    public:
        Grid();
        ~Grid() = default;
        
        const Matrix& ComputeWorldMatrix(Transform* camera);
        const auto& GetVertexBuffer()   const { return m_vertex_buffer; }
        const uint32_t GetVertexCount() const { return m_vertex_count; }

    private:
        void BuildGrid(std::vector<RHI_Vertex_PosCol>* vertices);

        uint32_t m_terrain_height = 200;
        uint32_t m_terrain_width  = 200;
        uint32_t m_vertex_count   = 0;

        std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;
        Matrix m_world;
    };
}
