
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
		cmd_list->SetConstantBuffer(Renderer_BindingsCb::frame, GetConstantBuffer(Renderer_ConstantBuffer::Frame));
		cmd_list->SetConstantBuffer(Renderer_BindingsCb::light, GetConstantBuffer(Renderer_ConstantBuffer::Light));
		cmd_list->SetConstantBuffer(Renderer_BindingsCb::material, GetConstantBuffer(Renderer_ConstantBuffer::Material));

		// textures todo: 暂时没有
		/*cmd_list->SetTexture(Renderer_BindingsSrv::noise_normal, GetStandardTexture(Renderer_StandardTexture::Noise_normal));
		cmd_list->SetTexture(Renderer_BindingsSrv::noise_blue, GetStandardTexture(Renderer_StandardTexture::Noise_blue));*/
	}
	
	void Renderer::Pass_ShadowMaps(RHI_CommandList* cmd_list, RendererPath* rendererPath, const bool is_transparent_pass)
	{
		// All objects are rendered from the lights point of view.
		// Opaque objects write their depth information to a depth buffer, using just a vertex shader.
		// Transparent objects read the opaque depth but don't write their own, instead, they write their color information using a pixel shader.

		auto camera = rendererPath->GetRenderCamera();
		auto rendererables = rendererPath->GetRenderables();

		// Acquire shaders
		RHI_Shader* shader_v = GetShader(Renderer_Shader::depth_light_V).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::depth_light_p).get();
		if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
			return;

		// Get entities
		vector<GameObject*>& entities = rendererables[is_transparent_pass ? Renderer_Entity::GeometryTransparent : Renderer_Entity::Geometry];
		if (entities.empty())
			return;

		cmd_list->BeginTimeblock(is_transparent_pass ? "shadow_maps_color" : "shadow_maps_depth");

		// Go through all of the lights
		const auto& entities_light = rendererables[Renderer_Entity::Light];
		for (uint32_t light_index = 0; light_index < entities_light.size(); light_index++)
		{
			Light* light = entities_light[light_index]->GetComponent<Light>();

			// Can happen when loading a new scene and the lights get deleted
			if (!light)
				continue;

			// Skip lights which don't cast shadows or have an intensity of zero
			if (!light->GetShadowsEnabled() || light->GetIntensityWatt(camera) == 0.0f)
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
			pso.shader_vertex = shader_v;
			pso.shader_pixel = is_transparent_pass ? shader_p : nullptr;
			pso.blend_state = is_transparent_pass ? GetBlendState(Renderer_BlendState::Alpha).get() : GetBlendState(Renderer_BlendState::Disabled).get();
			pso.depth_stencil_state = is_transparent_pass ? GetDepthStencilState(Renderer_DepthStencilState::Depth_read).get() : GetDepthStencilState(Renderer_DepthStencilState::Depth_read_write_stencil_read).get();
			pso.render_target_color_textures[0] = tex_color; // always bind so we can clear to white (in case there are no transparent objects)
			pso.render_target_depth_texture = tex_depth;
			pso.primitive_topology = RHI_PrimitiveTopology_Mode::TriangleList;
			pso.name = "Pass_ShadowMaps";

			for (uint32_t array_index = 0; array_index < tex_depth->GetArrayLength(); array_index++)
			{
				// Set render target texture array index
				pso.render_target_color_texture_array_index = array_index;
				pso.render_target_depth_stencil_texture_array_index = array_index;

				// Set clear values
				pso.clear_color[0] = Color::standard_white;
				pso.clear_depth = is_transparent_pass ? rhi_depth_load : 0.0f; // reverse-z

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
				bool render_pass_active = false;

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

					// todo:
					// Bind material (only for transparents)
					//if (is_transparent_pass)
					//{
					//	// Bind material textures
					//	RHI_Texture* tex_albedo = material->GetTexture(MaterialTexture::Color);
					//	cmd_list->SetTexture(Renderer_BindingsSrv::tex, tex_albedo ? tex_albedo : GetStandardTexture(Renderer_StandardTexture::White).get());

					//	// Set pass constants with material properties
					//	UpdateConstantBufferMaterial(cmd_list, material);
					//}

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

	void Renderer::Pass_ForwardPass(RHI_CommandList* cmd_list, RendererPath* rendererPath, const bool is_transparent_pass)
	{
		// acquire shaderer
		RHI_Shader* shader_v = GetShader(Renderer_Shader::forward_v).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::forward_p).get();

		if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
		{
			DEBUG_LOG_ERROR("Pass_ForwardPass Shader not Compiled !");
			return;
		}

		auto camera = rendererPath->GetRenderCamera();

		// acquire entities
		// auto& entities = m_renderables[Renderer_Entity::Geometry];
		auto& entities = rendererPath->GetRenderables().at(Renderer_Entity::Geometry);
		if (entities.empty())
		{
			return;
		}

		cmd_list->BeginTimeblock(!is_transparent_pass ? "Pass_ForwardPass" : "Pass_ForwardPass_Transparent");
		
	
		// define PipelineState
		static RHI_PipelineState pso;
		pso.name = !is_transparent_pass ? "Pass_ForwardPass" : "Pass_ForwardPass_Transparent";
		pso.shader_vertex = shader_v;
		pso.shader_pixel = shader_p;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Solid_cull_back).get();
		pso.blend_state = GetBlendState(Renderer_BlendState::Disabled).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Depth_read_write_stencil_read).get();

		// pso.render_target_depth_texture = GetRenderTarget(Renderer_RenderTexture::gbuffer_depth).get();// 不需要输出深度蒙版缓冲
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();// 不需要输出深度蒙版缓冲

		// pso.render_target_color_textures[0] = GetRenderTarget(Renderer_RenderTexture::frame_output).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.clear_depth = 0.0f; // reverse-z
		pso.clear_color[0] = Color::standard_blue; // reverse-z
		pso.primitive_topology = RHI_PrimitiveTopology_Mode::TriangleList;

		// begin render pass
		cmd_list->SetPipelineState(pso);
		cmd_list->BeginRenderPass();

		// 绘制所有的实体
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

			pso.shader_vertex = material->GetVertexShader();
			pso.shader_pixel = material->GetPixelShader();
			cmd_list->SetPipelineState(pso);

			// skip objects outside of the view frustum
			if (!camera->IsInViewFrustum(renderable))
			{
				// DEBUG_LOG_INFO("Renderer::Pass_ForwardPass Object Not InViewFrustum, name:{}", entity->GetName());
				continue;
			}

			//if (!render_pass_active)
			//{
			//	cmd_list->BeginRenderPass();
			//	render_pass_active = true;
			//}

			// Bind geometry
			cmd_list->SetBufferIndex(mesh->GetIndexBuffer());
			cmd_list->SetBufferVertex(mesh->GetVertexBuffer());

			// if (bound_material_id != material->GetObjectId())
			//{
				// todo: 暂时不绑定
				// BindTexturesMaterial(cmd_list, material);
			// UpdateConstantBufferMaterial(cmd_list, material);
			UpdateMaterialGlobalBuffer(cmd_list, material);
			// bound_material_id = material->GetObjectId();
			// }


			// Set pass constants with cascade transform
			m_cb_pass_cpu.transform = entity->GetComponent<Transform>()->GetMatrix();
			PushPassConstants(cmd_list);

			// Draw 
			cmd_list->DrawIndexed(renderable->GetIndexCount(), renderable->GetIndexOffset(), renderable->GetVertexOffset());
		}

		cmd_list->EndRenderPass();
		cmd_list->EndTimeblock();

	}

	void Renderer::Pass_DebugGridPass(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		RHI_Shader* shader_v = GetShader(Renderer_Shader::line_v).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::line_p).get();

		auto camera = rendererPath->GetRenderCamera();

		// define the pipeline state
		static RHI_PipelineState pso;
		pso.shader_vertex = shader_v;
		pso.shader_pixel = shader_p;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Wireframe_cull_none).get();
		// pso.render_target_color_textures[0] = GetRenderTarget(Renderer_RenderTexture::frame_output).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.clear_color[0] = rhi_color_load;
	/*	pso.render_target_color_textures[1] = tex_reactive_mask;
		pso.clear_color[1] = rhi_color_load;*/
		// pso.render_target_depth_texture = GetRenderTarget(Renderer_RenderTexture::gbuffer_depth).get();
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();
		pso.primitive_topology = RHI_PrimitiveTopology_Mode::LineList;

		cmd_list->BeginMarker("DebugGridPass");

		// set pipeline state
		pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Depth_read).get();

		cmd_list->SetPipelineState(pso);
		cmd_list->BeginRenderPass();
		// push pass constants
		{
			m_cb_pass_cpu.set_resolution_out(GetResolutionRender());
			if (camera)
			{
				m_cb_pass_cpu.transform = m_world_grid->ComputeWorldMatrix(camera->GetPosition());
			}
			PushPassConstants(cmd_list);
		}
		cmd_list->SetBufferVertex(m_world_grid->GetVertexBuffer().get());
		cmd_list->Draw(m_world_grid->GetVertexCount());
		cmd_list->EndRenderPass();
		cmd_list->EndMarker();

	}
	
}
