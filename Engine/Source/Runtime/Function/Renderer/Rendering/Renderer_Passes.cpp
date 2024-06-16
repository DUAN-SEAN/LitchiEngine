
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
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include "Runtime/Function/Framework/Component/UI/UIText.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RHI/RHI_ConstantBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_IndexBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_InputLayout.h"
#include "Runtime/Function/UI/Widgets/Texts/Text.h"
#include "Runtime/Function/UI/Widgets/Visual/Image.h"
//==============================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
	namespace 
	{
		// called by: Pass_ShadowMaps(), Pass_Depth_Prepass(), Pass_GBuffer()
		void draw_renderable(RHI_CommandList* cmd_list, RHI_PipelineState& pso,RendererPath* rendererPath, GameObject* entity, uint32_t array_index = 0)
		{
			RenderCamera* camera = rendererPath->GetRenderCamera();

			MeshFilter* renderable = entity->GetComponent<MeshFilter>();
			MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
			SkinnedMeshRenderer* skinned_mesh_renderer = entity->GetComponent<SkinnedMeshRenderer>();

			if (skinned_mesh_renderer)
			{
				meshRenderer = skinned_mesh_renderer;
			}

			if (!meshRenderer)
				return;

			// Skip meshes that don't cast shadows
			if (!meshRenderer->GetCastShadows())
				return;

			// Acquire geometry
			Mesh* mesh = renderable->GetMesh();
			if (!mesh || !mesh->GetVertexBuffer() || !mesh->GetIndexBuffer())
				return;

			// Acquire material
			Material* material = meshRenderer->GetMaterial();
			if (!material)
				return;

			// Skip objects outside of the view frustum
			if (!rendererPath->GetRenderCamera()->IsInViewFrustum(renderable))
				return;


			// Bind geometry
			cmd_list->SetBufferIndex(mesh->GetIndexBuffer());
			cmd_list->SetBufferVertex(mesh->GetVertexBuffer());

			// 如果是skinnedMesh 更新蒙皮数据
			if (skinned_mesh_renderer)
			{
				auto boneCbuffer = skinned_mesh_renderer->GetBoneConstantBuffer();
				cmd_list->SetConstantBuffer(Renderer_BindingsCb::boneArr, boneCbuffer);
			}

			cmd_list->DrawIndexed(renderable->GetIndexCount(), renderable->GetIndexOffset(), renderable->GetVertexOffset());
		}

	}
	void Renderer::SetStandardResources(RHI_CommandList* cmd_list)
	{
		EASY_FUNCTION(profiler::colors::Magenta)
		// constant buffers
		cmd_list->SetConstantBuffer(Renderer_BindingsCb::frame, GetConstantBuffer(Renderer_ConstantBuffer::Frame));
		//cmd_list->SetConstantBuffer(Renderer_BindingsCb::lightArr, GetConstantBuffer(Renderer_ConstantBuffer::LightArr));
		cmd_list->SetConstantBuffer(Renderer_BindingsCb::rendererPath, GetConstantBuffer(Renderer_ConstantBuffer::RendererPath));

		// textures todo: 
		/*cmd_list->SetTexture(Renderer_BindingsSrv::noise_normal, GetStandardTexture(Renderer_StandardTexture::Noise_normal));
		cmd_list->SetTexture(Renderer_BindingsSrv::noise_blue, GetStandardTexture(Renderer_StandardTexture::Noise_blue));*/
	}
	
	void Renderer::Pass_ShadowMaps(RHI_CommandList* cmd_list, RendererPath* rendererPath, const bool is_transparent_pass)
	{
		// All objects are rendered from the lights point of view.
		// Opaque objects write their depth information to a depth buffer, using just a vertex shader.
		// Transparent objects read the opaque depth but don't write their own, instead, they write their color information using a pixel shader.
		auto camera = rendererPath->GetRenderCamera();
		auto& rendererables = rendererPath->GetRenderables();

		// Acquire shaders
		RHI_Shader* shader_v = GetShader(Renderer_Shader::depth_light_V).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::depth_light_p).get();

		RHI_Shader* shader_skin_v = GetShader(Renderer_Shader::depth_light_skin_V).get();
		RHI_Shader* shader_skin_p = GetShader(Renderer_Shader::depth_light_skin_p).get();

		if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
			return;

		// Get entities
		const vector<GameObject*>& entities = rendererables[Renderer_Entity::Mesh];
		if (entities.empty())
			return;

		cmd_list->BeginTimeblock(is_transparent_pass ? "shadow_maps_color" : "shadow_maps_depth");

		// Define pipeline state
		static RHI_PipelineState pso;
		pso.blend_state = is_transparent_pass ? GetBlendState(Renderer_BlendState::Alpha).get() : GetBlendState(Renderer_BlendState::Off).get();
		pso.depth_stencil_state = is_transparent_pass ? GetDepthStencilState(Renderer_DepthStencilState::Read).get() : GetDepthStencilState(Renderer_DepthStencilState::ReadWrite).get();
		pso.name = "Pass_ShadowMaps";


		// Go through all of the lights
		const auto& rendererLightGroup = rendererPath->GetRendererLightGroup();
		const auto& lightCount = rendererLightGroup.GetLightCount();
		// Acquire light's shadow maps
		RHI_Texture* tex_depth = rendererLightGroup.m_texture_depth.get();
		RHI_Texture* tex_color = rendererLightGroup.m_texture_color.get();
		if (!tex_depth)
			return;

		pso.render_target_color_textures[0] = tex_color; // always bind so we can clear to white (in case there are no transparent objects)
		pso.render_target_depth_texture = tex_depth;

		// clear here and not via the render pass, which can dynamically start and end based on various toggles
		{
			if (pso.render_target_color_textures[0])
			{
				cmd_list->ClearRenderTarget(pso.render_target_color_textures[0], Color::standard_white);
			}
			cmd_list->ClearRenderTarget(pso.render_target_depth_texture, rhi_color_dont_care, 0.0f);
		}

		for (uint32_t light_index = 0; light_index < lightCount; light_index++)
		{
			const auto& lightData = rendererLightGroup.m_light_arr[light_index];
			Light* light = lightData.m_light;

			// Can happen when loading a new scene and the lights get deleted
			if (!light)
				continue;

			// Skip lights which don't cast shadows or have an intensity of zero
			if (!light->GetShadowsEnabled() || light->GetIntensityWatt(camera) == 0.0f)
				continue;

			// Skip lights that don't cast transparent shadows (if this is a transparent pass)
			if (is_transparent_pass && !light->GetShadowsTransparentEnabled())
				continue;

			// define light pso
			{
				if (light->GetLightType() == LightType::Directional)
				{
					// disable depth clipping so that we can capture silhouettes even behind the light
					pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Light_directional).get();
				}
				else
				{
					pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Light_point_spot).get();
				}

			}

			for (uint32_t array_index = 0; array_index < lightData.m_shadow_count; array_index++)
			{
				pso.render_target_array_index = 2 * light_index + array_index;

				bool needBeginRenderPass = true;

				if(!entities.empty())
				{
					int64_t index_start = !is_transparent_pass ? 0 : rendererPath->GetMeshIndexTransparent();
					int64_t index_end = !is_transparent_pass ? rendererPath->GetMeshIndexTransparent() : static_cast<int64_t>(rendererPath->GetRenderables()[Renderer_Entity::Mesh].size());

					// draw non skin go
					for (int i= index_start ; i<index_end;i++)
					{
						bool needs_pixel_shader =  is_transparent_pass;
						GameObject* entity = entities[i];
						if(SkinnedMeshRenderer* skinned_mesh_renderer = entity->GetComponent<SkinnedMeshRenderer>())
						{
							// draw has skin go
							pso.shader_vertex = shader_skin_v;
							pso.shader_pixel = needs_pixel_shader?shader_skin_p:nullptr;
						}else
						{
							// Set pipeline state
							pso.shader_vertex = shader_v;
							pso.shader_pixel = needs_pixel_shader?shader_p:nullptr;
						}


						cmd_list->SetPipelineState(pso, needBeginRenderPass);
						cmd_list->SetStructuredBuffer(Renderer_BindingsUav::sb_lights, rendererPath->GetLightBuffer());
						needBeginRenderPass = false;

						// Set pass constants with cascade transform
						m_cb_pass_cpu.set_light(static_cast<float>(array_index), static_cast<float>(light_index), lightCount);
						m_cb_pass_cpu.transform = entity->GetComponent<Transform>()->GetMatrix();

						/*m_pcb_pass_cpu.set_f3_value(
							material->HasTexture(MaterialTexture::AlphaMask) ? 1.0f : 0.0f,
							material->HasTexture(MaterialTexture::Color) ? 1.0f : 0.0f
						);*/

						PushPassConstants(cmd_list);

						draw_renderable(cmd_list, pso, rendererPath, entity);
					}
				}
				
			}
		}

		cmd_list->EndTimeblock();
	}

	void Renderer::Pass_SkyBox(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		cmd_list->BeginTimeblock("Pass_SkyBox");

		// define PipelineState
		auto camera = rendererPath->GetRenderCamera();
		static RHI_PipelineState pso;
		pso.name = "Pass_SkyBox";
		pso.shader_vertex = GetShader(Renderer_Shader::skybox_v).get();
		pso.shader_pixel = GetShader(Renderer_Shader::skybox_p).get();
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Solid_cull_none).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();
		pso.clear_color[0] = rhi_color_load;
		pso.primitive_topology = RHI_PrimitiveTopology::TriangleList;
		pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Read).get();
		cmd_list->SetPipelineState(pso);

		EASY_BLOCK("Render SkyBox")

		// set skyBox mesh
		cmd_list->SetBufferIndex(Renderer::m_index_buffer_skyBox.get());
		cmd_list->SetBufferVertex(Renderer::m_vertex_buffer_skyBox.get());

		// set skyBox cube
		cmd_list->SetTexture(Renderer_BindingsSrv::tex_skyBox, GetStandardTexture(Renderer_StandardTexture::SkyBox));

		// set skyBox Pos with current mainCamera's pos for render skybox
		m_cb_pass_cpu.transform = Matrix::CreateTranslation(camera->GetPosition());
		PushPassConstants(cmd_list);

		// drawCall
		cmd_list->DrawIndexed(Renderer::m_index_buffer_skyBox.get()->GetIndexCount(), 0, 0);

		EASY_END_BLOCK

		cmd_list->EndTimeblock();
	}

	void Renderer::Pass_ForwardPass(RHI_CommandList* cmd_list, RendererPath* rendererPath, const bool is_transparent_pass)
	{
		auto camera = rendererPath->GetRenderCamera();
		auto& rendererables = rendererPath->GetRenderables();

		// Get entities
		const vector<GameObject*>& entities = rendererables[Renderer_Entity::Mesh];
		if (entities.empty())
			return;


		cmd_list->BeginTimeblock(!is_transparent_pass ? "Pass_ForwardPass" : "Pass_ForwardPass_Transparent");

		// deduce rasterizer state
		bool is_wireframe = GetOption<bool>(Renderer_Option::Wireframe);
		RHI_RasterizerState* rasterizer_state = is_wireframe ? GetRasterizerState(Renderer_RasterizerState::Wireframe_cull_none).get() : GetRasterizerState(Renderer_RasterizerState::Solid_cull_back).get();
	
		// define PipelineState
		static RHI_PipelineState pso;
		pso.name = !is_transparent_pass ? "Pass_ForwardPass" : "Pass_ForwardPass_Transparent";
		//pso.shader_vertex = shader_v;
		//pso.shader_pixel = shader_p; // 
		pso.rasterizer_state = rasterizer_state;
		pso.blend_state = is_transparent_pass ? GetBlendState(Renderer_BlendState::Alpha).get() : GetBlendState(Renderer_BlendState::Off).get();
		pso.depth_stencil_state = is_transparent_pass ? GetDepthStencilState(Renderer_DepthStencilState::Read).get() : GetDepthStencilState(Renderer_DepthStencilState::ReadWrite).get();
		// pso.render_target_depth_texture = GetRenderTarget(Renderer_RenderTarget::forward_pass_depth).get();// 不需要输出深度蒙版缓冲
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();// 不需要输出深度蒙版缓冲
		// pso.render_target_color_textures[0] = GetRenderTarget(Renderer_RenderTarget::frame_output).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.clear_depth = is_transparent_pass? rhi_depth_dont_care :0.0f; // reverse-z
		//pso.clear_color[0] = camera->GetClearColor();
		pso.primitive_topology = RHI_PrimitiveTopology::TriangleList;

		EASY_BLOCK("Render Entities")

		bool needBeginRenderPass = true;
		int64_t index_start = !is_transparent_pass ? 0 : rendererPath->GetMeshIndexTransparent();
		int64_t index_end = !is_transparent_pass ? rendererPath->GetMeshIndexTransparent() : static_cast<int64_t>(rendererPath->GetRenderables()[Renderer_Entity::Mesh].size());
		const auto& rendererLightGroup = rendererPath->GetRendererLightGroup();
		const auto& lightCount = rendererLightGroup.GetLightCount();
		for (int64_t i = index_start; i < index_end; i++)
		{
			GameObject* entity = entities[i];
		
			EASY_BLOCK("Render Entity")
			EASY_BLOCK("Prevoius SetPSO")
			// Acquire renderable component
			MeshFilter* renderable = entity->GetComponent<MeshFilter>();
			SkinnedMeshRenderer* skinnedMeshRenderer = entity->GetComponent<SkinnedMeshRenderer>();
			MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();

			if (skinnedMeshRenderer)
			{
				meshRenderer = skinnedMeshRenderer;
			}

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

			// skip objects outside of the view frustum
			if (!camera->IsInViewFrustum(renderable))
			{
				// DEBUG_LOG_INFO("Renderer::Pass_ForwardPass Object Not InViewFrustum, name:{}", entity->GetName());
				continue;
			}
			EASY_END_BLOCK

			EASY_BLOCK("SetPipelineState")
			pso.shader_vertex = material->GetVertexShader();
			pso.shader_pixel = material->GetPixelShader();
			pso.material_shader = material->GetShader();
			cmd_list->SetPipelineState(pso, needBeginRenderPass);
			needBeginRenderPass = false;
			EASY_END_BLOCK

			EASY_BLOCK("SetBuffer")
			// Bind geometry
			cmd_list->SetBufferIndex(mesh->GetIndexBuffer());
			cmd_list->SetBufferVertex(mesh->GetVertexBuffer());
			EASY_END_BLOCK

			EASY_BLOCK("SetMaterialBuffer")
			SetMaterialBuffer(cmd_list, material);
			EASY_END_BLOCK

			// 如果是skinnedMesh 更新蒙皮数据
			if(skinnedMeshRenderer)
			{
				auto boneCbuffer = skinnedMeshRenderer->GetBoneConstantBuffer();
				cmd_list->SetConstantBuffer(Renderer_BindingsCb::boneArr, boneCbuffer);
			}

			if(lightCount>0)
			{
				// just main light
				cmd_list->SetTexture(Renderer_BindingsSrv::light_directional_depth, rendererLightGroup.m_texture_depth);
				m_cb_pass_cpu.set_light(static_cast<float>(0), static_cast<float>(0), lightCount);
				cmd_list->SetStructuredBuffer(Renderer_BindingsUav::sb_lights, rendererPath->GetLightBuffer());
				
			}

			EASY_BLOCK("PushPassConstants")
			m_cb_pass_cpu.set_resolution_out(rendererPath->GetDepthRenderTarget().get());
			m_cb_pass_cpu.set_is_transparent(is_transparent_pass);
			m_cb_pass_cpu.transform = entity->GetComponent<Transform>()->GetMatrix();
			PushPassConstants(cmd_list);
			EASY_END_BLOCK

			EASY_BLOCK("DrawCall")
			// Draw 
			cmd_list->DrawIndexed(renderable->GetIndexCount(), renderable->GetIndexOffset(), renderable->GetVertexOffset());
			EASY_END_BLOCK
			EASY_END_BLOCK
		}
		EASY_END_BLOCK

		cmd_list->EndTimeblock();
	}

	void Renderer::Pass_UIPass(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		// get common shader todo: temp not use material

		auto shader_v_font = GetShader(Renderer_Shader::font_v).get();
		auto shader_p_font = GetShader(Renderer_Shader::font_p).get();
		auto shader_v_image = GetShader(Renderer_Shader::ui_image_v).get();
		auto shader_p_image = GetShader(Renderer_Shader::ui_image_p).get();
		if (!shader_v_font || !shader_v_font->IsCompiled() || !shader_p_font || !shader_p_font->IsCompiled())
			return;
		if (!shader_v_image || !shader_v_image->IsCompiled() || !shader_p_image || !shader_p_image->IsCompiled())
			return;

		// todo image shader

		cmd_list->BeginMarker("UI");

		// define pipeline state
		static RHI_PipelineState pso;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Solid_cull_back).get();
		pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Off).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.primitive_topology = RHI_PrimitiveTopology::TriangleList;
		pso.name = "Pass_UI";

		auto& entities = rendererPath->GetRenderables().at(Renderer_Entity::UI);
		for (auto entity : entities)
		{
			auto text = entity->GetComponent<UIText>();
			auto image = entity->GetComponent<UIImage>();

			if(text)
			{
				auto font = text->GetFont();

				auto vertexBuffer = text->GetVertexBuffer().get();
				auto indexBuffer = text->GetIndexBuffer().get();
				if(!vertexBuffer||!indexBuffer || indexBuffer->GetIndexCount()==0)
				{
					continue;
				}

				pso.shader_vertex = shader_v_font;
				pso.shader_pixel = shader_p_font;
				cmd_list->SetPipelineState(pso);

				// set pass constants
				// m_cb_pass_cpu.transform  UI Transform
				m_cb_pass_cpu.transform = entity->GetComponent<Transform>()->GetMatrix();

				//m_cb_pass_cpu.set_resolution_out(tex_out);
				m_cb_pass_cpu.set_f4_value(text->GetColor());
				PushPassConstants(cmd_list);

				cmd_list->SetBufferVertex(vertexBuffer);
				cmd_list->SetBufferIndex(indexBuffer);
				cmd_list->SetTexture(Renderer_BindingsSrv::font_atlas, font->GetAtlas());
				cmd_list->DrawIndexed(indexBuffer->GetIndexCount());
			}

			if(image)
			{
				auto vertexBuffer = image->GetVertexBuffer().get();
				auto indexBuffer = image->GetIndexBuffer().get();
				if (!vertexBuffer || !indexBuffer || indexBuffer->GetIndexCount() == 0)
				{
					continue;
				}

				pso.shader_vertex = shader_v_image;
				pso.shader_pixel = shader_p_image;
				cmd_list->SetPipelineState(pso);

				// set pass constants
				// m_cb_pass_cpu.transform  UI Transform
				m_cb_pass_cpu.set_resolution_out(rendererPath->GetDepthRenderTarget().get());
				m_cb_pass_cpu.transform = entity->GetComponent<Transform>()->GetMatrix();

				//m_cb_pass_cpu.set_resolution_out(tex_out);
				m_cb_pass_cpu.set_f4_value(image->GetColor());
				PushPassConstants(cmd_list);

				cmd_list->SetBufferVertex(vertexBuffer);
				cmd_list->SetBufferIndex(indexBuffer);
				cmd_list->SetTexture(Renderer_BindingsSrv::tex, image->GetTexture());
				cmd_list->DrawIndexed(indexBuffer->GetIndexCount());
			}
		}

		cmd_list->EndMarker();
	}

	void Renderer::Pass_GridPass(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		// acquire resources
		RHI_Shader* shader_v = GetShader(Renderer_Shader::grid_v).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::grid_p).get();
		if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
			return;

		cmd_list->BeginTimeblock("grid");

		// set pipeline state
		static RHI_PipelineState pso;
		pso.shader_vertex = shader_v;
		pso.shader_pixel = shader_p;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Solid_cull_none).get();
		pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Read).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();
		cmd_list->SetPipelineState(pso);

		// set transform
		{
			// follow camera in world unit increments so that the grid appears stationary in relation to the camera
			const float grid_spacing = 1.0f;
			const Vector3& camera_position = rendererPath->GetRenderCamera()->GetPosition();
			const Vector3 translation = Vector3(
				floor(camera_position.x / grid_spacing) * grid_spacing,
				0.0f,
				floor(camera_position.z / grid_spacing) * grid_spacing
			);

			m_cb_pass_cpu.transform = Matrix::CreateScale(Vector3(1000.0f, 1.0f, 1000.0f)) * Matrix::CreateTranslation(translation);
			cmd_list->PushConstants(m_cb_pass_cpu);
		}

		cmd_list->SetBufferVertex(GetStandardMesh(Renderer_MeshType::Quad)->GetVertexBuffer());
		cmd_list->SetBufferIndex(GetStandardMesh(Renderer_MeshType::Quad)->GetIndexBuffer());
		cmd_list->DrawIndexed(6);

		cmd_list->EndTimeblock();
	}

	void Renderer::Pass_LinesPass(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		// acquire resources
		RHI_Shader* shader_v = GetShader(Renderer_Shader::line_v).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::line_p).get();
		if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
			return;

		cmd_list->BeginTimeblock("lines");

		// set pipeline state
		static RHI_PipelineState pso;
		pso.shader_vertex = shader_v;
		pso.shader_pixel = shader_p;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Wireframe_cull_none).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.clear_color[0] = rhi_color_load;
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();
		pso.primitive_topology = RHI_PrimitiveTopology::LineList;

		// world space rendering
		m_cb_pass_cpu.transform = Matrix::Identity;
		cmd_list->PushConstants(m_cb_pass_cpu);

		// draw independent lines
		const bool draw_lines_depth_off = m_lines_index_depth_off != numeric_limits<uint32_t>::max();
		const bool draw_lines_depth_on = m_lines_index_depth_on > ((m_line_vertices.size() / 2) - 1);
		if (draw_lines_depth_off || draw_lines_depth_on)
		{
			cmd_list->SetCullMode(RHI_CullMode::None);

			// grow vertex buffer (if needed)
			uint32_t vertex_count = static_cast<uint32_t>(m_line_vertices.size());
			if (vertex_count > m_vertex_buffer_lines->GetVertexCount())
			{
				m_vertex_buffer_lines->CreateDynamic<RHI_Vertex_PosCol>(vertex_count);
			}

			if (vertex_count != 0)
			{
				// update vertex buffer
				RHI_Vertex_PosCol* buffer = static_cast<RHI_Vertex_PosCol*>(m_vertex_buffer_lines->GetMappedData());
				copy(m_line_vertices.begin(), m_line_vertices.end(), buffer);

				// depth off
				if (draw_lines_depth_off)
				{
					cmd_list->BeginMarker("depth_off");

					// set pipeline state
					pso.blend_state = GetBlendState(Renderer_BlendState::Off).get();
					pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Off).get();
					cmd_list->SetPipelineState(pso);

					cmd_list->SetBufferVertex(m_vertex_buffer_lines.get());
					cmd_list->Draw(m_lines_index_depth_off + 1);

					cmd_list->EndMarker();
				}

				// depth on
				if (m_lines_index_depth_on > (vertex_count / 2) - 1)
				{
					cmd_list->BeginMarker("depth_on");

					// set pipeline state
					pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
					pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Read).get();
					cmd_list->SetPipelineState(pso);

					cmd_list->SetBufferVertex(m_vertex_buffer_lines.get());
					cmd_list->Draw((m_lines_index_depth_on - (vertex_count / 2)) + 1, vertex_count / 2);

					cmd_list->EndMarker();
				}
			}
		}

		m_lines_index_depth_off = numeric_limits<uint32_t>::max();                         // max +1 will wrap it to 0
		m_lines_index_depth_on = (static_cast<uint32_t>(m_line_vertices.size()) / 2) - 1; // -1 because +1 will make it go to size / 2

		cmd_list->EndTimeblock();
	}

	void Renderer::Pass_IconPass(RHI_CommandList* cmd_list, RendererPath* rendererPath, Cb_RendererPath& rendererPathBufferData)
	{

		auto camera = rendererPath->GetRenderCamera();
		if(camera==nullptr)
		{
			return;
		}

		// acquire shaders
		RHI_Shader* shader_v = GetShader(Renderer_Shader::quad_v).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::quad_p).get();
		if (!shader_v->IsCompiled() || !shader_p->IsCompiled())
			return;

		// acquire entities
		auto& lights = rendererPath->GetRenderables()[Renderer_Entity::Light];
		auto& cameras = rendererPath->GetRenderables()[Renderer_Entity::Camera];
		auto& audio_sources = rendererPath->GetRenderables()[Renderer_Entity::AudioSource];
		if ((lights.empty() && audio_sources.empty())&& cameras.empty()|| !rendererPath->GetRenderCamera())
			return;

		cmd_list->BeginTimeblock("icons");

		// define pipeline state
		static RHI_PipelineState pso;
		pso.shader_vertex = shader_v;
		pso.shader_pixel = shader_p;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Solid_cull_none).get();
		pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Off).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();

		// set pipeline state
		cmd_list->SetPipelineState(pso);

		auto draw_icon = [&cmd_list, &rendererPathBufferData, camera](GameObject* entity, RHI_Texture* texture)
			{
				const Vector3 pos_world = entity->GetComponent<Transform>()->GetPosition();
				const Vector3 pos_world_camera = camera->GetPosition();
				const Vector3 camera_to_light = (pos_world - pos_world_camera).Normalized();
				const float v_dot_l = Vector3::Dot(camera->GetForward(), camera_to_light);

				// only draw if it's inside our view
				if (v_dot_l > 0.5f)
				{
					// compute transform
					{
						// use the distance from the camera to scale the icon, this will
						// cancel out perspective scaling, hence keeping the icon scale constant
						const float distance = (pos_world_camera - pos_world).Length();
						const float scale = distance * 0.04f;

						// 1st rotation: The quad's normal is parallel to the world's Y axis, so we rotate to make it camera facing
						Quaternion rotation_reorient_quad = Quaternion::FromEulerAngles(-90.0f, 0.0f, 0.0f);
						// 2nd rotation: Rotate the camera facing quad with the camera, so that it remains a camera facing quad
						Quaternion rotation_camera_billboard = Quaternion::FromLookRotation(pos_world - pos_world_camera);

						//auto xAngle = 0.0f;
						//auto xAxis = Vector3::Right;
						//entity->GetComponent<Transform>()->GetRotation().ToAngleAxis(xAngle, xAxis);
						//Quaternion::FromAngleAxis(xAngle, xAxis)

						Matrix transform = Matrix(pos_world, rotation_camera_billboard * rotation_reorient_quad, scale);

						// set transform
						m_cb_pass_cpu.transform = transform * rendererPathBufferData.view_projection_unjittered;
						cmd_list->PushConstants(m_cb_pass_cpu);
					}

					// draw rectangle
					cmd_list->SetTexture(Renderer_BindingsSrv::tex, texture);
					cmd_list->SetBufferVertex(GetStandardMesh(Renderer_MeshType::Quad)->GetVertexBuffer());
					cmd_list->SetBufferIndex(GetStandardMesh(Renderer_MeshType::Quad)->GetIndexBuffer());
					cmd_list->DrawIndexed(6);
				}
			};

		// draw audio source icons
		for (GameObject* entity : audio_sources)
		{
			draw_icon(entity, GetStandardTexture(Renderer_StandardTexture::Gizmo_audio_source).get());
		}

		// draw audio source icons
		for (GameObject* entity : cameras)
		{
			draw_icon(entity, GetStandardTexture(Renderer_StandardTexture::Gizmo_camera).get());
		}

		// draw light icons
		for (GameObject* entity : lights)
		{
			RHI_Texture* texture = nullptr;

			// light can be null if it just got removed and our buffer doesn't update till the next frame
			if (auto light = entity->GetComponent<Light>())
			{
				// get the texture
				if (light->GetLightType() == LightType::Directional) texture = GetStandardTexture(Renderer_StandardTexture::Gizmo_light_directional).get();
				else if (light->GetLightType() == LightType::Point)  texture = GetStandardTexture(Renderer_StandardTexture::Gizmo_light_point).get();
				else if (light->GetLightType() == LightType::Spot)   texture = GetStandardTexture(Renderer_StandardTexture::Gizmo_light_spot).get();
			}

			draw_icon(entity, texture);
		}

		cmd_list->EndTimeblock();
	}

	void Renderer::Pass_SelectedAssetViewResourcePass(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		Material* selectMaterial = rendererPath->GetSelectedMaterial();

		// todo now only support posUvNorTan
		if(selectMaterial!=nullptr && selectMaterial->GetShader()->m_vertex_shader->GetInputLayout()->GetVertexType() != RHI_Vertex_Type::PosUvNorTan)
		{
			return;
		}

		Mesh* selectedMesh = rendererPath->GetSelectedMesh();
		RHI_Texture2D* selectedTexture2d = rendererPath->GetSelectedTexture2D();
		SelectedResourceType selectedResType  = rendererPath->GetSelectedResourceType();

		RHI_VertexBuffer* m_vertex_buffer;
		RHI_IndexBuffer* m_index_buffer;
		int indexCount;
		Matrix transform = Matrix::CreateScale(1) * Matrix::CreateTranslation(Vector3::Zero) * Matrix::CreateRotation(Quaternion::Identity);
		std::shared_ptr<RHI_ConstantBuffer> selectedMeshBoneConstantBuffer;

		auto worldMatrix = Matrix::CreateScale(1) * Matrix::CreateTranslation(Vector3::Zero) * Matrix::CreateRotation(Quaternion::Identity);

		switch (selectedResType) {
		case SelectedResourceType_None:
			return;
		case SelectedResourceType_Material:
			m_vertex_buffer = GetStandardMesh(Renderer_MeshType::Sphere)->GetVertexBuffer();
			m_index_buffer = GetStandardMesh(Renderer_MeshType::Sphere)->GetIndexBuffer();
			indexCount = GetStandardMesh(Renderer_MeshType::Sphere)->GetIndexCount();
			transform = worldMatrix;
			break;
		case SelectedResourceType_Mesh:
			m_vertex_buffer = selectedMesh->GetVertexBuffer();
			m_index_buffer = selectedMesh->GetIndexBuffer();
			indexCount = selectedMesh->GetIndexCount();
			// transform = ;
			selectMaterial = selectedMesh->IsAnimationModel() ? m_default_standard_skin_material : m_default_standard_material;
			selectedMeshBoneConstantBuffer = rendererPath->GetSelectedMeshBoneConstantBuffer();
			break;
		case SelectedResourceType_Texture2D:

			m_vertex_buffer = GetStandardMesh(Renderer_MeshType::Quad)->GetVertexBuffer();
			m_index_buffer = GetStandardMesh(Renderer_MeshType::Quad)->GetIndexBuffer();
			indexCount = GetStandardMesh(Renderer_MeshType::Quad)->GetIndexCount();
			transform = worldMatrix;
			selectMaterial = m_default_standard_material;
			selectMaterial->SetTexture("u_diffuseMap", selectedTexture2d);
			break;
		}


		if(selectMaterial == nullptr && selectedMesh==nullptr && selectedTexture2d)
		{
			return;
		}

		// set material default
		selectMaterial->Tick();

		RHI_Shader* shader_v = selectMaterial->GetVertexShader();
		RHI_Shader* shader_p = selectMaterial->GetPixelShader();

		auto camera = rendererPath->GetRenderCamera();
		const auto& rendererLightGroup = rendererPath->GetRendererLightGroup();
		const auto& lightCount = rendererLightGroup.GetLightCount();

		// define PipelineState
		static RHI_PipelineState pso;
		pso.name = "Pass_MaterialPass";
		pso.shader_vertex = shader_v;
		pso.shader_pixel = shader_p;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Solid_cull_back).get();
		pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::ReadWrite).get();
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.clear_depth = 0.0f; // reverse-z
		pso.clear_color[0] = camera->GetClearColor();
		pso.primitive_topology = RHI_PrimitiveTopology::TriangleList;
		pso.material_shader = selectMaterial->GetShader();

		cmd_list->BeginMarker("Pass_SelectedAssetViewResourcePass");

		// set pipeline state
		cmd_list->SetPipelineState(pso,true);

		// push pass constants
		{
			/*m_cb_pass_cpu.set_resolution_out(GetResolutionRender());*/
			EASY_BLOCK("PushPassConstants")
			// Set pass constants with cascade transform
			m_cb_pass_cpu.transform = transform;
			m_cb_pass_cpu.set_light(static_cast<float>(0), static_cast<float>(0), lightCount);
			PushPassConstants(cmd_list);
			EASY_END_BLOCK
		}
		cmd_list->SetBufferVertex(m_vertex_buffer);
		cmd_list->SetBufferIndex(m_index_buffer);

		// if skin set bone buffer
		if (selectedResType == SelectedResourceType_Mesh && selectedMesh->IsAnimationModel())
		{
			cmd_list->SetConstantBuffer(Renderer_BindingsCb::boneArr, selectedMeshBoneConstantBuffer);
		}

		// set material buffer
		EASY_BLOCK("SetMaterialBuffer")
		SetMaterialBuffer(cmd_list, selectMaterial);
		EASY_END_BLOCK

		// set light  buffer
		cmd_list->SetStructuredBuffer(Renderer_BindingsUav::sb_lights, rendererPath->GetLightBuffer());

		EASY_BLOCK("DrawCall")
		// Draw 
		if(selectedResType == SelectedResourceType_Mesh)
		{
			const auto& subMeshArr = selectedMesh->GetSubMeshArr();

			LC_ASSERT_MSG(subMeshArr.size() > 0, "SubMeshSize ==0")

			if(subMeshArr.size()>0)
			{
				for (const auto& subMesh : subMeshArr)
				{
					cmd_list->DrawIndexed(subMesh.m_geometryIndexCount, subMesh.m_geometryIndexOffset, subMesh.m_geometryVertexOffset);
				}
			}

		}else
		{
			cmd_list->DrawIndexed(indexCount, 0, 0);
		}
		EASY_END_BLOCK

		cmd_list->EndMarker();
	}
}
