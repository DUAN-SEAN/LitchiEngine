
#pragma once

//= INCLUDES ===============
#include "RHI_Definitions.h"
#include <array>
//==========================

namespace LitchiRuntime
{
    class SP_CLASS RHI_PipelineState
    {
    public:
        RHI_PipelineState();
        ~RHI_PipelineState();

        uint64_t ComputeHash();
        uint64_t GetHash()    const { return m_hash; }
        uint32_t GetWidth()   const;
        uint32_t GetHeight()  const;
        bool IsValid()        const;
        bool HasClearValues() const;
        bool IsGraphics()     const { return (shader_vertex != nullptr || shader_pixel != nullptr) && !shader_compute; }
        bool IsCompute()      const { return shader_compute != nullptr && !IsGraphics(); }
        
        //= STATIC - Can cause PSO generation ================================================
        RHI_Shader* shader_vertex                     = nullptr;
        RHI_Shader* shader_pixel                      = nullptr;
        RHI_Shader* shader_compute                    = nullptr;
        RHI_RasterizerState* rasterizer_state         = nullptr;
        RHI_BlendState* blend_state                   = nullptr;
        RHI_DepthStencilState* depth_stencil_state    = nullptr;
        RHI_SwapChain* render_target_swapchain        = nullptr;
        RHI_PrimitiveTopology_Mode primitive_topology = RHI_PrimitiveTopology_Mode::Undefined;
        bool can_use_vertex_index_buffers             = true;
        bool dynamic_scissor                          = false;
        //  bool instancing                               = false;

        // RTs
        RHI_Texture* render_target_depth_texture = nullptr;
        std::array<RHI_Texture*, rhi_max_render_target_count> render_target_color_textures;
        uint32_t render_target_color_texture_array_index         = 0;
        uint32_t render_target_depth_stencil_texture_array_index = 0;
        //====================================================================================

        //= DYNAMIC - Will not cause PSO generation ===============
        float clear_depth      = rhi_depth_load;
        uint32_t clear_stencil = rhi_stencil_load;
        std::array<Color, rhi_max_render_target_count> clear_color;

        bool render_target_depth_texture_read_only = false;

        std::string name;
        //=========================================================

    private:
        uint64_t m_hash                   = 0;
        RHI_PipelineState* m_pso_previous = nullptr;
    };
}
