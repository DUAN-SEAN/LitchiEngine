
//= INCLUDES =====================
#include "Runtime/Core/pch.h"
#include "RHI_PipelineState.h"
#include "RHI_Shader.h"
#include "RHI_Texture.h"
#include "RHI_SwapChain.h"
#include "RHI_BlendState.h"
#include "RHI_InputLayout.h"
#include "RHI_RasterizerState.h"
#include "RHI_DepthStencilState.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
//================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
    namespace
    {
        void validate(RHI_PipelineState& pso)
        {
            bool has_shader_compute = pso.shader_compute ? pso.shader_compute->IsCompiled() : false;
            bool has_shader_vertex = pso.shader_vertex ? pso.shader_vertex->IsCompiled() : false;
            bool has_shader_pixel = pso.shader_pixel ? pso.shader_pixel->IsCompiled() : false;
            bool has_render_target = pso.render_target_color_textures[0] || pso.render_target_depth_texture; // check that there is at least one render target
            bool has_backbuffer = pso.render_target_swapchain;                                            // check that no both the swapchain and the color render target are active
            bool has_graphics_states = pso.rasterizer_state && pso.blend_state && pso.depth_stencil_state;
            bool is_graphics = (has_shader_vertex || has_shader_pixel) && !has_shader_compute;
            bool is_compute = has_shader_compute && (!has_shader_vertex && !has_shader_pixel);

            LC_ASSERT_MSG(has_shader_compute || has_shader_vertex || has_shader_pixel, "There must be at least one shader");
            if (is_graphics)
            {
                LC_ASSERT_MSG(has_graphics_states, "Graphics states are missing");
                LC_ASSERT_MSG(has_render_target || has_backbuffer, "A render target is missing");
                LC_ASSERT(pso.GetWidth() != 0 && pso.GetHeight() != 0);
            }
        }

        uint64_t compute_hash(RHI_PipelineState& pso)
        {
            uint64_t hash = 0;

            hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.instancing));
            hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.primitive_topology));

            if (pso.render_target_swapchain)
            {
                hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.render_target_swapchain->GetFormat()));
            }

            if (pso.rasterizer_state)
            {
                hash = rhi_hash_combine(hash, pso.rasterizer_state->GetHash());
            }

            if (pso.blend_state)
            {
                hash = rhi_hash_combine(hash, pso.blend_state->GetHash());
            }

            if (pso.depth_stencil_state)
            {
                hash = rhi_hash_combine(hash, pso.depth_stencil_state->GetHash());
            }

            // shaders
            {
                if (pso.shader_vertex)
                {
                    hash = rhi_hash_combine(hash, pso.shader_vertex->GetHash());
                }

                if (pso.shader_hull)
                {
                    hash = rhi_hash_combine(hash, pso.shader_hull->GetHash());
                }

                if (pso.shader_domain)
                {
                    hash = rhi_hash_combine(hash, pso.shader_domain->GetHash());
                }

                if (pso.shader_pixel)
                {
                    hash = rhi_hash_combine(hash, pso.shader_pixel->GetHash());
                }

                if (pso.shader_compute)
                {
                    hash = rhi_hash_combine(hash, pso.shader_compute->GetHash());
                }
            }

            // rt
            {
                // color
                for (uint32_t i = 0; i < rhi_max_render_target_count; i++)
                {
                    if (RHI_Texture* texture = pso.render_target_color_textures[i])
                    {
                        hash = rhi_hash_combine(hash, static_cast<uint64_t>(texture->GetFormat()));
                    }
                }

                // depth
                if (pso.render_target_depth_texture)
                {
                    hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.render_target_depth_texture->GetFormat()));
                }

                //// variable rate shading
                //if (pso.vrs_input_texture)
                //{
                //    hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.vrs_input_texture->GetFormat()));
                //}
            }

            return hash;
        }

        uint64_t compute_hash_dynamic(RHI_PipelineState& pso)
        {
            uint64_t hash = 0;

            hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.render_target_color_texture_array_index));
            hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.render_target_depth_stencil_texture_array_index));
            hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.clear_depth));
            hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.clear_stencil));
            for (uint32_t i = 0; i < rhi_max_render_target_count; i++)
            {
                hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.clear_color[i].r));
                hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.clear_color[i].g));
                hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.clear_color[i].b));
                hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.clear_color[i].a));
            }
            hash = rhi_hash_combine(hash, static_cast<uint64_t>(pso.resolution_scale));

            return hash;
        }

        void get_dimensions(RHI_PipelineState& pso, uint32_t* width, uint32_t* height)
        {
            LC_ASSERT(width && height);

            *width = 0;
            *height = 0;

            if (pso.render_target_swapchain)
            {
                if (width)  *width = pso.render_target_swapchain->GetWidth();
                if (height) *height = pso.render_target_swapchain->GetHeight();
            }
            else if (pso.render_target_color_textures[0])
            {
                if (width)  *width = pso.render_target_color_textures[0]->GetWidth();
                if (height) *height = pso.render_target_color_textures[0]->GetHeight();
            }
            else if (pso.render_target_depth_texture)
            {
                if (width)  *width = pso.render_target_depth_texture->GetWidth();
                if (height) *height = pso.render_target_depth_texture->GetHeight();
            }

            if (pso.resolution_scale)
            {
                float resolution_scale = Renderer::GetOption<float>(Renderer_Option::ResolutionScale);
                *width = static_cast<uint32_t>(*width * resolution_scale);
                *height = static_cast<uint32_t>(*height * resolution_scale);
            }
        }
    }

    RHI_PipelineState::RHI_PipelineState()
    {
        clear_color.fill(rhi_color_load);
        render_target_color_textures.fill(nullptr);
    }

    RHI_PipelineState::~RHI_PipelineState()
    {

    }

    void RHI_PipelineState::Prepare()
    {
        m_hash = compute_hash(*this);
        m_hash_dynamic = compute_hash_dynamic(*this);

        get_dimensions(*this, &m_width, &m_height);
        validate(*this);
    }

    bool RHI_PipelineState::HasClearValues() const
    {
        if (clear_depth != rhi_depth_load && clear_depth != rhi_depth_dont_care)
            return true;

        if (clear_stencil != rhi_stencil_load && clear_stencil != rhi_stencil_dont_care)
            return true;

        for (const Color& color : clear_color)
        {
            if (color != rhi_color_load && color != rhi_color_dont_care)
                return true;
        }

        return false;
    }
}
