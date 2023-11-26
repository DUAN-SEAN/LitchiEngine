
#pragma once

//= INCLUDES ==================================
#include <array>
//#include "Runtime/Core/Event.h"
#include "Runtime/Function/Renderer/RHI/RHI_Device.h"
#include "Runtime/Function/Renderer/RHI/RHI_Shader.h"
#include "imgui.h"
#include "Runtime/Function/Renderer/RHI/RHI_Semaphore.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2D.h"
#include "Runtime/Function/Renderer/RHI/RHI_SwapChain.h"
#include "Runtime/Function/Renderer/RHI/RHI_BlendState.h"
#include "Runtime/Function/Renderer/RHI/RHI_CommandPool.h"
#include "Runtime/Function/Renderer/RHI/RHI_CommandList.h"
#include "Runtime/Function/Renderer/RHI/RHI_IndexBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_VertexBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_PipelineState.h"
#include "Runtime/Function/Renderer/RHI/RHI_ConstantBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_RasterizerState.h"
#include "Runtime/Function/Renderer/RHI/RHI_DepthStencilState.h"
#include "Runtime/Function/Renderer/Rendering/Renderer_ConstantBuffers.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
#include "Runtime/Function/Renderer/Resource/ResourceCache.h"
//#include "../../Widgets/TextureViewer.h"
#include "ImGui_RHI.h"
#include "Runtime/Core/App/ApplicationBase.h"
//=============================================

namespace ImGui::RHI
{
    //= NAMESPACES =========
    using namespace LitchiRuntime;
    using namespace Math;
    using namespace std;
    //======================

    static const uint32_t buffer_count = 6;

    struct ViewportRhiResources
    {
        RHI_CommandPool* cmd_pool = nullptr;
        array<unique_ptr<RHI_IndexBuffer>,  buffer_count> index_buffers;
        array<unique_ptr<RHI_VertexBuffer>, buffer_count> vertex_buffers;
        Pcb_Pass push_constant_buffer_pass;
        uint32_t buffer_index = 0;
        
        ViewportRhiResources() = default;
        ViewportRhiResources(const char* name, RHI_SwapChain* swapchain)
        {
            // allocate command pool
            cmd_pool = RHI_Device::CommandPoolAllocate(name, swapchain->GetObjectId(), RHI_Queue_Type::Graphics);

            // allocate buffers
            for (uint32_t i = 0; i < buffer_count; i++)
            {
                index_buffers[i]  = make_unique<RHI_IndexBuffer>(true, name);
                vertex_buffers[i] = make_unique<RHI_VertexBuffer>(true, name);
            }
        }
    };

    struct WindowData
    {
        shared_ptr<ViewportRhiResources> viewport_rhi_resources;
        shared_ptr<RHI_SwapChain>        swapchain;
    };

    // forward declarations
    void initialize_platform_interface();

    // main window rhi resources
    ViewportRhiResources g_viewport_data;

    // shared rhi resources (between all windows)
    shared_ptr<RHI_Texture>           g_font_atlas;
    shared_ptr<RHI_DepthStencilState> g_depth_stencil_state;
    shared_ptr<RHI_RasterizerState>   g_rasterizer_state;
    shared_ptr<RHI_BlendState>        g_blend_state;
    shared_ptr<RHI_Shader>            g_shader_vertex;
    shared_ptr<RHI_Shader>            g_shader_pixel;

    static void destroy_rhi_resources()
    {
        g_font_atlas          = nullptr;
        g_depth_stencil_state = nullptr;
        g_rasterizer_state    = nullptr;
        g_blend_state         = nullptr;
        g_shader_vertex       = nullptr;
        g_shader_pixel        = nullptr;

        for (auto& ptr : g_viewport_data.index_buffers)
        {
            ptr = nullptr;
        }

        for (auto& ptr : g_viewport_data.vertex_buffers)
        {
            ptr = nullptr;
        }
    }

    static void Initialize()
    {
        // create required RHI objects
        {
            g_viewport_data = ViewportRhiResources("imgui", Renderer::GetSwapChain());

            g_depth_stencil_state = make_shared<RHI_DepthStencilState>(false, false, RHI_Comparison_Function::Always);

            g_rasterizer_state = make_shared<RHI_RasterizerState>
            (
                RHI_CullMode::None,
                RHI_PolygonMode::Solid,
                true,  // depth clip
                true,  // scissor
                false, // multi-sample
                false  // anti-aliased lines
            );

            g_blend_state = make_shared<RHI_BlendState>
            (
                true,
                RHI_Blend::Src_Alpha,     // source blend
                RHI_Blend::Inv_Src_Alpha, // destination blend
                RHI_Blend_Operation::Add, // blend op
                RHI_Blend::Inv_Src_Alpha, // source blend alpha
                RHI_Blend::Zero,          // destination blend alpha
                RHI_Blend_Operation::Add  // destination op alpha
            );

            // compile shaders
            {
                const string shader_path = ResourceCache::GetResourceDirectory(ResourceDirectory::Shaders) + "\\ImGui.hlsl";

                bool async = false;

                g_shader_vertex = make_shared<RHI_Shader>();
                g_shader_vertex->Compile(RHI_Shader_Vertex, shader_path, async, RHI_Vertex_Type::Pos2dUvCol8);

                g_shader_pixel = make_shared<RHI_Shader>();
                g_shader_pixel->Compile(RHI_Shader_Pixel, shader_path, async);
            }
        }

        // font atlas
        {
            unsigned char* pixels = nullptr;
            int atlas_width       = 0;
            int atlas_height      = 0;
            int bpp               = 0;
            ImGuiIO& io           = GetIO();
            io.Fonts->GetTexDataAsRGBA32(&pixels, &atlas_width, &atlas_height, &bpp);

            // copy pixel data
            vector<RHI_Texture_Slice> texture_data;
            vector<std::byte>& mip = texture_data.emplace_back().mips.emplace_back().bytes;
            const uint32_t size = atlas_width * atlas_height * bpp;
            mip.resize(size);
            mip.reserve(size);
            memcpy(&mip[0], reinterpret_cast<std::byte*>(pixels), size);

            // upload texture to graphics system
            g_font_atlas = make_shared<RHI_Texture2D>(atlas_width, atlas_height, RHI_Format::R8G8B8A8_Unorm, RHI_Texture_Srv, texture_data, "imgui_font_atlas");
            io.Fonts->TexID = static_cast<ImTextureID>(g_font_atlas.get());
        }

        // setup back-end capabilities flags
        ImGuiIO& io             = GetIO();
        io.BackendFlags        |= ImGuiBackendFlags_RendererHasViewports;
        io.BackendFlags        |= ImGuiBackendFlags_RendererHasVtxOffset;
        io.BackendRendererName  = "RHI";
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            initialize_platform_interface();
        }

        // SP_SUBSCRIBE_TO_EVENT(EventType::RendererOnShutdown, SP_EVENT_HANDLER_STATIC(destroy_rhi_resources));
    }

    static void shutdown()
    {
        DestroyPlatformWindows();
    }

    static void render(ImDrawData* draw_data, WindowData* window_data = nullptr, const bool clear = true)
    {
        // EASY_FUNCTION(profiler::colors::Magenta);
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width  = static_cast<int>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = static_cast<int>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        // get the viewport resources
        bool is_child_window                = window_data != nullptr;
        ViewportRhiResources* rhi_resources = is_child_window ? window_data->viewport_rhi_resources.get() : &g_viewport_data;

        rhi_resources->cmd_pool->Tick();

        // get buffer index
        uint32_t buffer_index       = rhi_resources->buffer_index;
        rhi_resources->buffer_index = (rhi_resources->buffer_index + 1) % buffer_count;

        // get rhi resources for this command buffer
        RHI_VertexBuffer* vertex_buffer = rhi_resources->vertex_buffers[buffer_index].get();
        RHI_IndexBuffer* index_buffer   = rhi_resources->index_buffers[buffer_index].get();
        RHI_CommandList* cmd_list       = rhi_resources->cmd_pool->GetCurrentCommandList();

        // update vertex and index buffers
        {
            // grow vertex buffer as needed
            if (vertex_buffer->GetVertexCount() < static_cast<uint32_t>(draw_data->TotalVtxCount))
            {
                const uint32_t vertex_count     = vertex_buffer->GetVertexCount();
                const uint32_t vertex_count_new = draw_data->TotalVtxCount + 15000;
                vertex_buffer->CreateDynamic<ImDrawVert>(vertex_count_new);

                if (vertex_count != 0)
                {
                    DEBUG_LOG_INFO("Vertex buffer has been re-allocated to fit %d vertices", vertex_count_new);
                }
            }

            // grow index buffer as needed
            if (index_buffer->GetIndexCount() < static_cast<uint32_t>(draw_data->TotalIdxCount))
            {
                const uint32_t index_count     = index_buffer->GetIndexCount();
                const uint32_t index_count_new = draw_data->TotalIdxCount + 30000;
                index_buffer->CreateDynamic<ImDrawIdx>(index_count_new);

                if (index_count != 0)
                {
                    DEBUG_LOG_INFO("Index buffer has been re-allocated to fit %d indices", index_count_new);
                }
            }

            // copy all imgui vertices into a single buffer
            ImDrawVert* vtx_dst = static_cast<ImDrawVert*>(vertex_buffer->GetMappedData());
            ImDrawIdx*  idx_dst = static_cast<ImDrawIdx*>(index_buffer->GetMappedData());
            if (vtx_dst && idx_dst)
            {
                for (auto i = 0; i < draw_data->CmdListsCount; i++)
                {
                    const ImDrawList* imgui_cmd_list = draw_data->CmdLists[i];

                    memcpy(vtx_dst, imgui_cmd_list->VtxBuffer.Data, imgui_cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
                    memcpy(idx_dst, imgui_cmd_list->IdxBuffer.Data, imgui_cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

                    vtx_dst += imgui_cmd_list->VtxBuffer.Size;
                    idx_dst += imgui_cmd_list->IdxBuffer.Size;
                }
            }
        }

        // define pipeline state
        static RHI_PipelineState pso = {};
        pso.name                     = "imgui";
        pso.shader_vertex            = g_shader_vertex.get();
        pso.shader_pixel             = g_shader_pixel.get();
        pso.rasterizer_state         = g_rasterizer_state.get();
        pso.blend_state              = g_blend_state.get();
        pso.depth_stencil_state      = g_depth_stencil_state.get();
        pso.render_target_swapchain  = is_child_window ? window_data->swapchain.get() : Renderer::GetSwapChain();
        pso.clear_color[0]           = clear ? Color::standard_black : rhi_color_dont_care;
        pso.dynamic_scissor          = true;
        pso.primitive_topology       = RHI_PrimitiveTopology_Mode::TriangleList;

        // begin
        const char* name = is_child_window ? "imgui_window_child" : "imgui_window_main";
        bool gpu_timing  = !is_child_window; // profiler requires more work when windows enter the main window and their command pool is destroyed
        cmd_list->Begin();

        // do layout transitions
        {
            // transitions have to happen outside of the render pass
            for (uint32_t i = 0; i < static_cast<uint32_t>(draw_data->CmdListsCount); i++)
            {
                ImDrawList* cmd_list_imgui = draw_data->CmdLists[i];

                for (uint32_t cmd_i = 0; cmd_i < static_cast<uint32_t>(cmd_list_imgui->CmdBuffer.Size); cmd_i++)
                {
                    const ImDrawCmd* pcmd = &cmd_list_imgui->CmdBuffer[cmd_i];

                    if (RHI_Texture* texture = static_cast<RHI_Texture*>(pcmd->TextureId))
                    {
                        // transition will happen only if needed
                        texture->SetLayout(LitchiRuntime::RHI_Image_Layout::Shader_Read_Only_Optimal, cmd_list);
                    }
                }
            }
        }

        cmd_list->BeginTimeblock(name, true, gpu_timing);
        cmd_list->SetBufferVertex(vertex_buffer);
        cmd_list->SetBufferIndex(index_buffer);
        cmd_list->SetPipelineState(pso);
        cmd_list->BeginRenderPass();

        // render
        {
            uint32_t global_vtx_offset = 0;
            uint32_t global_idx_offset = 0;
            for (uint32_t i = 0; i < static_cast<uint32_t>(draw_data->CmdListsCount); i++)
            {
                ImDrawList* cmd_list_imgui = draw_data->CmdLists[i];

                for (uint32_t cmd_i = 0; cmd_i < static_cast<uint32_t>(cmd_list_imgui->CmdBuffer.Size); cmd_i++)
                {
                    const ImDrawCmd* pcmd = &cmd_list_imgui->CmdBuffer[cmd_i];

                    if (pcmd->UserCallback != nullptr)
                    {
                        pcmd->UserCallback(cmd_list_imgui, pcmd);
                    }
                    else
                    {
                        // set scissor rectangle
                        {
                            Rectangle scissor_rect;
                            scissor_rect.left   = pcmd->ClipRect.x - draw_data->DisplayPos.x;
                            scissor_rect.top    = pcmd->ClipRect.y - draw_data->DisplayPos.y;
                            scissor_rect.right  = pcmd->ClipRect.z - draw_data->DisplayPos.x;
                            scissor_rect.bottom = pcmd->ClipRect.w - draw_data->DisplayPos.y;

                            cmd_list->SetScissorRectangle(scissor_rect);
                        }

                        // push pass/draw call constants
                        {
                            // set texture and update texture viewer parameters
                            {
                                bool m_channel_r           = true;
                                bool m_channel_g           = true;
                                bool m_channel_b           = true;
                                bool m_channel_a           = true;
                                bool m_gamma_correct       = true;
                                bool m_pack                = true;
                                bool m_boost               = true;
                                bool m_abs                 = true;
                                bool m_point_sampling      = true;
                                float mip_level            = 1;// 0
                                bool is_texture_visualised = false;

                                if (RHI_Texture* texture = static_cast<RHI_Texture*>(pcmd->TextureId))
                                {
                                    // during engine startup, some textures might be loading in different threads
                                    if (texture->IsReadyForUse())
                                    {
                                        cmd_list->SetTexture(Renderer_BindingsSrv::tex, texture);

                                        //// update texture viewer parameters
                                        //is_texture_visualised = TextureViewer::GetVisualisedTextureId() == texture->GetObjectId();
                                        //mip_level = static_cast<float>(is_texture_visualised ? TextureViewer::GetMipLevel() : 0);
                                        //if (is_texture_visualised)
                                        //{
                                        //    m_channel_r      = TextureViewer::GetVisualisationFlags() & Visualise_Channel_R;
                                        //    m_channel_g      = TextureViewer::GetVisualisationFlags() & Visualise_Channel_G;
                                        //    m_channel_b      = TextureViewer::GetVisualisationFlags() & Visualise_Channel_B;
                                        //    m_channel_a      = TextureViewer::GetVisualisationFlags() & Visualise_Channel_A;
                                        //    m_gamma_correct  = TextureViewer::GetVisualisationFlags() & Visualise_GammaCorrect;
                                        //    m_pack           = TextureViewer::GetVisualisationFlags() & Visualise_Pack;
                                        //    m_boost          = TextureViewer::GetVisualisationFlags() & Visualise_Boost;
                                        //    m_abs            = TextureViewer::GetVisualisationFlags() & Visualise_Abs;
                                        //    m_point_sampling = TextureViewer::GetVisualisationFlags() & Visualise_Sample_Point;
                                        //}
                                    }
                                }

                                rhi_resources->push_constant_buffer_pass.set_f4_value(m_channel_r, m_channel_g, m_channel_b, m_channel_a);
                                rhi_resources->push_constant_buffer_pass.set_f3_value(m_gamma_correct, m_pack, m_boost);
                                rhi_resources->push_constant_buffer_pass.set_f3_value2(m_abs, m_point_sampling, mip_level);
                                rhi_resources->push_constant_buffer_pass.set_is_transparent(is_texture_visualised);
                            }

                            // compute transform matrix
                            {
                                const float L = draw_data->DisplayPos.x;
                                const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
                                const float T = draw_data->DisplayPos.y;
                                const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

                                rhi_resources->push_constant_buffer_pass.transform = Matrix
                                (
                                    2.0f / (R - L), 0.0f, 0.0f, (R + L) / (L - R),
                                    0.0f, 2.0f / (T - B), 0.0f, (T + B) / (B - T),
                                    0.0f, 0.0f, 0.5f, 0.5f,
                                    0.0f, 0.0f, 0.0f, 1.0f
                                );
                            }

                            cmd_list->PushConstants(0, sizeof(Pcb_Pass), &rhi_resources->push_constant_buffer_pass);
                        }

                        cmd_list->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
                    }
                }

                global_idx_offset += static_cast<uint32_t>(cmd_list_imgui->IdxBuffer.Size);
                global_vtx_offset += static_cast<uint32_t>(cmd_list_imgui->VtxBuffer.Size);
            }
        }

        // submit
        cmd_list->EndRenderPass();
        cmd_list->EndTimeblock();
        cmd_list->End();
        cmd_list->Submit();


        if (!is_child_window)
        {
            // todo: ��bug ��ע�͵�
        	// if (!ApplicationBase::Instance()->window->IsMinimized())
            {
                Renderer::Present();
            }
        }
    }

    static void window_create(ImGuiViewport* viewport)
    {
        // platformHandle is SDL_Window, PlatformHandleRaw is HWND
        SP_ASSERT_MSG(viewport->PlatformHandle != nullptr, "Platform handle is invalid");

        WindowData* window = new WindowData();
        window->swapchain = make_shared<RHI_SwapChain>
        (
            viewport->PlatformHandle,
            static_cast<uint32_t>(viewport->Size.x),
            static_cast<uint32_t>(viewport->Size.y),
            RHI_Present_Mode::Immediate,
            2,
            (string("swapchain_child_") + string(to_string(viewport->ID))).c_str()
        );

        window->viewport_rhi_resources = make_shared<ViewportRhiResources>("imgui_child_window", window->swapchain.get());
        viewport->RendererUserData     = window;
    }

    static void window_destroy(ImGuiViewport* viewport)
    {
        if (WindowData* window = static_cast<WindowData*>(viewport->RendererUserData))
        {
            RHI_Device::CommandPoolDestroy(window->viewport_rhi_resources->cmd_pool);
            delete window;
        }

        viewport->RendererUserData = nullptr;
    }

    static void window_resize(ImGuiViewport* viewport, const ImVec2 size)
    {
        static_cast<WindowData*>(viewport->RendererUserData)->swapchain->Resize(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
    }

    static void window_render(ImGuiViewport* viewport, void*)
    {
        const bool clear = !(viewport->Flags & ImGuiViewportFlags_NoRendererClear);
        render(viewport->DrawData, static_cast<WindowData*>(viewport->RendererUserData), clear);
    }

    static void window_present(ImGuiViewport* viewport, void*)
    {
        WindowData* window = static_cast<WindowData*>(viewport->RendererUserData);
        SP_ASSERT(window->viewport_rhi_resources->cmd_pool->GetCurrentCommandList()->GetState() == LitchiRuntime::RHI_CommandListState::Submitted);
        window->swapchain->Present();
    }

    inline void initialize_platform_interface()
    {
        ImGuiPlatformIO& platform_io       = ImGui::GetPlatformIO();
        platform_io.Renderer_CreateWindow  = window_create;
        platform_io.Renderer_DestroyWindow = window_destroy;
        platform_io.Renderer_SetWindowSize = window_resize;
        platform_io.Renderer_RenderWindow  = window_render;
        platform_io.Renderer_SwapBuffers   = window_present;
    }
}
