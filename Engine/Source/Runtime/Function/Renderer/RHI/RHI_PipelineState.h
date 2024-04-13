
#pragma once

//= INCLUDES ===============
#include "RHI_Definitions.h"
#include <array>

#include "Runtime/Function/Renderer/Rendering/MaterialShader.h"
#include "spdlog/fmt/bundled/chrono.h"
//==========================

namespace LitchiRuntime
{
    // todo: changed
    class LC_CLASS RHI_PipelineState
    {
    public:
        RHI_PipelineState();
        ~RHI_PipelineState();


        void Prepare();
        bool HasClearValues() const;

        uint64_t GetHash() const { return m_hash; }
        uint64_t GetHashDynamic() const { return m_hash_dynamic; }
        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }
        bool IsGraphics() const { return (shader_vertex != nullptr || shader_pixel != nullptr) && !shader_compute; }
        bool IsCompute() const { return shader_compute != nullptr && !IsGraphics(); }
        bool HasTessellation() const { return shader_domain != nullptr && shader_hull != nullptr; }
        
        //= STATIC - Can cause PSO generation ================================================
        MaterialShader* material_shader = nullptr; // pipeline Level
        RHI_Shader* shader_vertex                     = nullptr; // pipeline Level
        RHI_Shader* shader_hull                       = nullptr; // pipeline Level
        RHI_Shader* shader_domain                     = nullptr; // pipeline Level
        RHI_Shader* shader_pixel                      = nullptr; // pipeline Level
        RHI_Shader* shader_compute                    = nullptr; // pipeline Level
        RHI_RasterizerState* rasterizer_state         = nullptr; // pipeline Level
        RHI_BlendState* blend_state                   = nullptr; // pipeline Level
        RHI_DepthStencilState* depth_stencil_state    = nullptr; // renderPass pipeline Level
        RHI_SwapChain* render_target_swapchain        = nullptr; // renderPass pipeline Level
        RHI_PrimitiveTopology primitive_topology = RHI_PrimitiveTopology::TriangleList; // pipeline Level
        bool instancing                               = false; // pipeline Level
        bool resolution_scale = false; // renderPass pipeline Level

        // RTs
        std::array<RHI_Texture*, rhi_max_render_target_count> render_target_color_textures; // renderPass pipeline Level
        RHI_Texture* render_target_depth_texture = nullptr; // renderPass pipeline Level
        RHI_Texture* vrs_input_texture = nullptr; // renderPass pipeline Level
        //====================================================================================

        //= DYNAMIC - will not cause pso generation =================
        uint32_t render_target_color_texture_array_index = 0; // renderPass Level
        uint32_t render_target_depth_stencil_texture_array_index = 0; // renderPass Level

        // Clear Param
        float clear_depth      = rhi_depth_load; // renderPass Level
        uint32_t clear_stencil = rhi_stencil_load; // renderPass Level
        std::array<Color, rhi_max_render_target_count> clear_color; // renderPass Level

        std::string name;
        //=========================================================

    private:
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        uint64_t m_hash = 0;
        uint64_t m_hash_dynamic = 0;
        RHI_PipelineState* m_pso_previous = nullptr;
    };
}
