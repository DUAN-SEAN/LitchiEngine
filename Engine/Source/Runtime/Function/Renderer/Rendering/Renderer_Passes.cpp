
//= INCLUDES ===================================
#include "Runtime/Core/pch.h"
#include "Renderer.h"
#include "../RHI/RHI_CommandList.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_Shader.h"
#include "../RHI/RHI_AMD_FidelityFX.h"
#include "../RHI/RHI_StructuredBuffer.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
//==============================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
    namespace
    {
        mutex mutex_generate_mips;

        const float thread_group_count = 8.0f;
        #define thread_group_count_x(tex) static_cast<uint32_t>(Math::Helper::Ceil(static_cast<float>(tex->GetWidth())  / thread_group_count))
        #define thread_group_count_y(tex) static_cast<uint32_t>(Math::Helper::Ceil(static_cast<float>(tex->GetHeight()) / thread_group_count))
    }

    void Renderer::SetGlobalShaderResources(RHI_CommandList* cmd_list)
    {
        // constant buffers
        cmd_list->SetConstantBuffer(Renderer_BindingsCb::frame,    GetConstantBuffer(Renderer_ConstantBuffer::Frame));
        cmd_list->SetConstantBuffer(Renderer_BindingsCb::light,    GetConstantBuffer(Renderer_ConstantBuffer::Light));
        cmd_list->SetConstantBuffer(Renderer_BindingsCb::material, GetConstantBuffer(Renderer_ConstantBuffer::Material));

        // textures
        cmd_list->SetTexture(Renderer_BindingsSrv::noise_normal, GetStandardTexture(Renderer_StandardTexture::Noise_normal));
        cmd_list->SetTexture(Renderer_BindingsSrv::noise_blue,   GetStandardTexture(Renderer_StandardTexture::Noise_blue));
    }

    void Renderer::Pass_Frame(RHI_CommandList* cmd_list)
    {
        // SP_PROFILE_FUNCTION();

        // acquire render targets
        RHI_Texture* rt1       = GetRenderTarget(Renderer_RenderTexture::frame_render).get();
        RHI_Texture* rt2       = GetRenderTarget(Renderer_RenderTexture::frame_render_2).get();
        RHI_Texture* rt_output = GetRenderTarget(Renderer_RenderTexture::frame_output).get();

        // update frame constant buffer
        UpdateConstantBufferFrame(cmd_list);

        if (auto camera = GetCamera())
        { 
            // if there are no entities, clear to the camera's color
            if (GetEntities()[Renderer_Entity::Geometry].empty() && GetEntities()[Renderer_Entity::GeometryTransparent].empty() && GetEntities()[Renderer_Entity::Light].empty())
            {
                GetCmdList()->ClearRenderTarget(rt_output, 0, 0, false, camera->GetClearColor());
            }
            else // render frame
            {
                // generate brdf specular lut
                if (!m_brdf_specular_lut_rendered)
                {
                    // Pass_BrdfSpecularLut(cmd_list);
                    m_brdf_specular_lut_rendered = true;
                }

                // determine if a transparent pass is required
                const bool do_transparent_pass = !GetEntities()[Renderer_Entity::GeometryTransparent].empty();

                // shadow maps
                {
                    Pass_ShadowMaps(cmd_list, false);
                    if (do_transparent_pass)
                    {
                        Pass_ShadowMaps(cmd_list, true);
                    }
                }

                // Pass_ReflectionProbes(cmd_list);

                // opaque
                {
                    bool is_transparent_pass = false;

                    //Pass_Depth_Prepass(cmd_list);
                    //Pass_GBuffer(cmd_list, is_transparent_pass);
                    //Pass_Ssgi(cmd_list);
                    //Pass_Ssr(cmd_list, rt1);
                    //Pass_Light(cmd_list, is_transparent_pass);                  // compute diffuse and specular buffers
                    //Pass_Light_Composition(cmd_list, rt1, is_transparent_pass); // compose diffuse, specular, ssgi, volumetric etc.
                    //Pass_Light_ImageBased(cmd_list, rt1, is_transparent_pass);  // apply IBL and SSR
                }

                // transparent
                if (do_transparent_pass)
                {
                    // blit the frame so that refraction can sample from it
                    cmd_list->Copy(rt1, rt2, true);

                    // generate frame mips so that the reflections can simulate roughness
                    // Pass_Ffx_Spd(cmd_list, rt2);

                    // blur the smaller mips to reduce blockiness/flickering
                    for (uint32_t i = 1; i < rt2->GetMipCount(); i++)
                    {
                        const bool depth_aware = false;
                        const float radius     = 1.0f;
                        const float sigma      = 12.0f;
                        // Pass_Blur_Gaussian(cmd_list, rt2, depth_aware, radius, sigma, i);
                    }

                    bool is_transparent_pass = true;

                   /* Pass_GBuffer(cmd_list, is_transparent_pass);
                    Pass_Light(cmd_list, is_transparent_pass);
                    Pass_Light_Composition(cmd_list, rt1, is_transparent_pass);
                    Pass_Light_ImageBased(cmd_list, rt1, is_transparent_pass);*/
                }

                // Pass_PostProcess(cmd_list);
            }

            // editor related stuff - passes that render on top of each other
           /* Pass_DebugMeshes(cmd_list, rt_output);
            Pass_Icons(cmd_list, rt_output);
            Pass_PeformanceMetrics(cmd_list, rt_output);*/
        }
        else
        {
            // if there is no camera, clear to black and and render the performance metrics
            GetCmdList()->ClearRenderTarget(rt_output, 0, 0, false, Color::standard_black);
            // Pass_PeformanceMetrics(cmd_list, rt_output);
        }

        // transition the render target to a readable state so it can be rendered
        // within the viewport or copied to the swap chain back buffer
        rt_output->SetLayout(RHI_Image_Layout::Shader_Read_Only_Optimal, cmd_list);
    }

    void Renderer::Pass_ShadowMaps(RHI_CommandList* cmd_list, const bool is_transparent_pass)
    {
        // All objects are rendered from the lights point of view.
        // Opaque objects write their depth information to a depth buffer, using just a vertex shader.
        // Transparent objects read the opaque depth but don't write their own, instead, they write their color information using a pixel shader.

        // Acquire shaders
        RHI_Shader* shader_v = GetShader(Renderer_Shader::depth_light_V).get();
        RHI_Shader* shader_p = GetShader(Renderer_Shader::depth_light_p).get();
        if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
            return;

        // Get entities
        vector<GameObject*>& entities = m_renderables[is_transparent_pass ? Renderer_Entity::GeometryTransparent : Renderer_Entity::Geometry];
        if (entities.empty())
            return;

        cmd_list->BeginTimeblock(is_transparent_pass ? "shadow_maps_color" : "shadow_maps_depth");

        // Go through all of the lights
        const auto& entities_light = GetEntities()[Renderer_Entity::Light];
        for (uint32_t light_index = 0; light_index < entities_light.size(); light_index++)
        {
            Light* light = entities_light[light_index]->GetComponent<Light>();

            // Can happen when loading a new scene and the lights get deleted
            if (!light)
                continue;

            // Skip lights which don't cast shadows or have an intensity of zero
            if (!light->GetShadowsEnabled() || light->GetIntensityWatt(GetCamera().get()) == 0.0f)
                continue;

            // Skip lights that don't cast transparent shadows (if this is a transparent pass)
            if (is_transparent_pass && !light->GetShadowsTransparentEnabled())
                continue;

            // Acquire light's shadow maps
            RHI_Texture* tex_depth = light->GetDepthTexture();
            RHI_Texture* tex_color = light->GetColorTexture();
            if (!tex_depth)
                continue;

            // Define pipeline state
            static RHI_PipelineState pso;
            pso.shader_vertex                   = shader_v;
            pso.shader_pixel                    = is_transparent_pass ? shader_p : nullptr;
            pso.blend_state                     = is_transparent_pass ? GetBlendState(Renderer_BlendState::Alpha).get() : GetBlendState(Renderer_BlendState::Disabled).get();
            pso.depth_stencil_state             = is_transparent_pass ? GetDepthStencilState(Renderer_DepthStencilState::Depth_read).get() : GetDepthStencilState(Renderer_DepthStencilState::Depth_read_write_stencil_read).get();
            pso.render_target_color_textures[0] = tex_color; // always bind so we can clear to white (in case there are no transparent objects)
            pso.render_target_depth_texture     = tex_depth;
            pso.primitive_topology              = RHI_PrimitiveTopology_Mode::TriangleList;
            pso.name                            = "Pass_ShadowMaps";

            for (uint32_t array_index = 0; array_index < tex_depth->GetArrayLength(); array_index++)
            {
                // Set render target texture array index
                pso.render_target_color_texture_array_index         = array_index;
                pso.render_target_depth_stencil_texture_array_index = array_index;

                // Set clear values
                pso.clear_color[0] = Color::standard_white;
                pso.clear_depth    = is_transparent_pass ? rhi_depth_load : 0.0f; // reverse-z

                const Matrix& view_projection = light->GetViewMatrix(array_index) * light->GetProjectionMatrix(array_index);

                // Set appropriate rasterizer state
                if (light->GetLightType() == LightType::Directional)
                {
                    // "Pancaking" - https://www.gamedev.net/forums/topic/639036-shadow-mapping-and-high-up-objects/
                    // It's basically a way to capture the silhouettes of potential shadow casters behind the light's view point.
                    // Of course we also have to make sure that the light doesn't cull them in the first place (this is done automatically by the light)
                    pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Light_directional).get();
                }
                else
                {
                    pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Light_point_spot).get();
                }

                // Set pipeline state
                cmd_list->SetPipelineState(pso);

                // State tracking
                bool render_pass_active    = false;

                for (GameObject* entity : entities)
                {
                    // Acquire renderable component
                    MeshFilter* renderable = entity->GetComponent<MeshFilter>();
                    MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
                    if (!meshRenderer)
                        continue;

                    // Skip meshes that don't cast shadows
                    if (!meshRenderer->GetCastShadows())
                        continue;

                    // Acquire geometry
                    Mesh* mesh = renderable->GetMesh();
                    if (!mesh || !mesh->GetVertexBuffer() || !mesh->GetIndexBuffer())
                        continue;

                    // Acquire material
                    Material* material = meshRenderer->GetMaterial();
                    if (!material)
                        continue;

                    // Skip objects outside of the view frustum
                    if (!light->IsInViewFrustum(renderable, array_index))
                        continue;

                    if (!render_pass_active)
                    {
                        cmd_list->BeginRenderPass();
                        render_pass_active = true;
                    }

                    // Bind material (only for transparents)
                    if (is_transparent_pass)
                    {
                        // Bind material textures
                        RHI_Texture* tex_albedo = material->GetTexture(MaterialTexture::Color);
                        cmd_list->SetTexture(Renderer_BindingsSrv::tex, tex_albedo ? tex_albedo : GetStandardTexture(Renderer_StandardTexture::White).get());

                        // Set pass constants with material properties
                        UpdateConstantBufferMaterial(cmd_list, material);
                    }

                    // Bind geometry
                    cmd_list->SetBufferIndex(mesh->GetIndexBuffer());
                    cmd_list->SetBufferVertex(mesh->GetVertexBuffer());

                    // Set pass constants with cascade transform
                    m_cb_pass_cpu.transform = entity->GetComponent<Transform>()->GetMatrix() * view_projection;
                    PushPassConstants(cmd_list);

                    cmd_list->DrawIndexed(renderable->GetIndexCount(), renderable->GetIndexOffset(), renderable->GetVertexOffset());
                }

                if (render_pass_active)
                {
                    cmd_list->EndRenderPass();
                }
            }
        }

        cmd_list->EndTimeblock();
    }

    //void Renderer::Pass_GBuffer(RHI_CommandList* cmd_list, const bool is_transparent_pass)
    //{
    //    // acquire shaders
    //    RHI_Shader* shader_v = GetShader(Renderer_Shader::gbuffer_v).get();
    //    RHI_Shader* shader_p = GetShader(Renderer_Shader::gbuffer_p).get();
    //    if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
    //        return;

    //    cmd_list->BeginTimeblock(is_transparent_pass ? "g_buffer_transparent" : "g_buffer");

    //    // acquire render targets
    //    RHI_Texture* tex_albedo            = GetRenderTarget(Renderer_RenderTexture::gbuffer_albedo).get();
    //    RHI_Texture* tex_normal            = GetRenderTarget(Renderer_RenderTexture::gbuffer_normal).get();
    //    RHI_Texture* tex_material          = GetRenderTarget(Renderer_RenderTexture::gbuffer_material).get();
    //    RHI_Texture* tex_material_2        = GetRenderTarget(Renderer_RenderTexture::gbuffer_material_2).get();
    //    RHI_Texture* tex_velocity          = GetRenderTarget(Renderer_RenderTexture::gbuffer_velocity).get();
    //    RHI_Texture* tex_velocity_previous = GetRenderTarget(Renderer_RenderTexture::gbuffer_velocity_previous).get();
    //    RHI_Texture* tex_depth             = GetRenderTarget(Renderer_RenderTexture::gbuffer_depth).get();
    //    RHI_Texture* tex_fsr2_transparency = GetRenderTarget(Renderer_RenderTexture::fsr2_mask_transparency).get();

    //    // copy velocity to previous velocity
    //    cmd_list->Blit(tex_velocity, tex_velocity_previous, false);

    //    bool depth_prepass = GetOption<bool>(Renderer_Option::DepthPrepass);
    //    bool wireframe     = GetOption<bool>(Renderer_Option::Debug_Wireframe);

    //    // deduce rasterizer state
    //    RHI_RasterizerState* rasterizer_state = is_transparent_pass ? GetRasterizerState(Renderer_RasterizerState::Solid_cull_none).get() : GetRasterizerState(Renderer_RasterizerState::Solid_cull_back).get();
    //    rasterizer_state                      = wireframe ? GetRasterizerState(Renderer_RasterizerState::Wireframe_cull_none).get() : rasterizer_state;

    //    // deduce depth-stencil state
    //    RHI_DepthStencilState* depth_stencil_state = depth_prepass ? GetDepthStencilState(Renderer_DepthStencilState::Depth_read).get() : GetDepthStencilState(Renderer_DepthStencilState::Depth_read_write_stencil_read).get();
    //    depth_stencil_state                        = is_transparent_pass ? GetDepthStencilState(Renderer_DepthStencilState::Depth_read).get() : depth_stencil_state;

    //    // clearing to zero, this will draw the sky
    //    static Color clear_color = Color(0.0f, 0.0f, 0.0f, 0.0f);

    //    // define pipeline state
    //    RHI_PipelineState pso;
    //    pso.name                            = "g_buffer";
    //    pso.shader_vertex                   = shader_v;
    //    pso.shader_pixel                    = shader_p;
    //    pso.blend_state                     = GetBlendState(Renderer_BlendState::Disabled).get();
    //    pso.rasterizer_state                = rasterizer_state;
    //    pso.depth_stencil_state             = depth_stencil_state;
    //    pso.render_target_color_textures[0] = tex_albedo;
    //    pso.clear_color[0]                  = is_transparent_pass ? rhi_color_load : Color::standard_transparent;
    //    pso.render_target_color_textures[1] = tex_normal;
    //    pso.clear_color[1]                  = pso.clear_color[0];
    //    pso.render_target_color_textures[2] = tex_material;
    //    pso.clear_color[2]                  = pso.clear_color[0];
    //    pso.render_target_color_textures[3] = tex_material_2;
    //    pso.clear_color[3]                  = pso.clear_color[0];
    //    pso.render_target_color_textures[4] = tex_velocity;
    //    pso.clear_color[4]                  = pso.clear_color[0];
    //    pso.render_target_color_textures[5] = tex_fsr2_transparency;
    //    pso.clear_color[5]                  = pso.clear_color[0];
    //    pso.render_target_depth_texture     = tex_depth;
    //    pso.clear_depth                     = (is_transparent_pass || depth_prepass) ? rhi_depth_load : 0.0f; // reverse-z
    //    pso.primitive_topology              = RHI_PrimitiveTopology_Mode::TriangleList;

    //    // set pipeline state
    //    cmd_list->SetPipelineState(pso);

    //    auto& entities = m_renderables[is_transparent_pass ? Renderer_Entity::GeometryTransparent : Renderer_Entity::Geometry];

    //    // render
    //    cmd_list->BeginRenderPass();
    //    {
    //        uint64_t bound_material_id = 0;

    //        for (shared_ptr<Entity> entity : entities)
    //        {
    //            // get renderable
    //            shared_ptr<Renderable> renderable = entity->GetComponent<Renderable>();
    //            if (!renderable)
    //                continue;

    //            // get material
    //            Material* material = renderable->GetMaterial();
    //            if (!material)
    //                continue;

    //            // Get geometry
    //            Mesh* mesh = renderable->GetMesh();
    //            if (!mesh || !mesh->GetVertexBuffer() || !mesh->GetIndexBuffer())
    //                continue;

    //            // skip objects outside of the view frustum
    //            if (!GetCamera()->IsInViewFrustum(renderable))
    //                continue;

    //            // set geometry (will only happen if not already set)
    //            cmd_list->SetBufferIndex(mesh->GetIndexBuffer());
    //            cmd_list->SetBufferVertex(mesh->GetVertexBuffer());

    //            // update material
    //            if (bound_material_id != material->GetObjectId())
    //            {
    //                // set textures
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_albedo,    material->GetTexture(MaterialTexture::Color));
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_roughness, material->GetTexture(MaterialTexture::Roughness));
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_metallic,  material->GetTexture(MaterialTexture::Metalness));
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_normal,    material->GetTexture(MaterialTexture::Normal));
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_height,    material->GetTexture(MaterialTexture::Height));
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_occlusion, material->GetTexture(MaterialTexture::Occlusion));
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_emission,  material->GetTexture(MaterialTexture::Emission));
    //                cmd_list->SetTexture(Renderer_BindingsSrv::material_mask,      material->GetTexture(MaterialTexture::AlphaMask));

    //                // set properties
    //                UpdateConstantBufferMaterial(cmd_list, material);

    //                bound_material_id = material->GetObjectId();
    //            }

    //            // push pass constants
    //            {
    //                m_cb_pass_cpu.set_is_transparent(is_transparent_pass);

    //                // update transform
    //                if (shared_ptr<Transform> transform = entity->GetTransform())
    //                {
    //                    m_cb_pass_cpu.transform = transform->GetMatrix();
    //                    m_cb_pass_cpu.set_transform_previous(transform->GetMatrixPrevious());

    //                    // save matrix for velocity computation
    //                    transform->SetMatrixPrevious(m_cb_pass_cpu.transform);
    //                }

    //                PushPassConstants(cmd_list);
    //            }

    //            // draw
    //            cmd_list->DrawIndexed(renderable->GetIndexCount(), renderable->GetIndexOffset(), renderable->GetVertexOffset());
    //            Profiler::m_renderer_meshes_rendered++;
    //        }

    //        cmd_list->EndRenderPass();
    //    }

    //    cmd_list->EndTimeblock();
    //}
    
}
