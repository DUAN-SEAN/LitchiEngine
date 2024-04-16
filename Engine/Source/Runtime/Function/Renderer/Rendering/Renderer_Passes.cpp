
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
			if (!rendererPath->IsInLightViewFrustum(renderable, array_index))
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
		const vector<GameObject*>& entities = rendererables[is_transparent_pass ? Renderer_Entity::GeometryTransparent : Renderer_Entity::Geometry];
		if (entities.empty())
			return;

		cmd_list->BeginTimeblock(is_transparent_pass ? "shadow_maps_color" : "shadow_maps_depth");

		// Go through all of the lights
		const auto& entities_light = rendererables[Renderer_Entity::Light];
		size_t lightCount = entities_light.size();
		for (uint32_t light_index = 0; light_index < lightCount; light_index++)
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
			RHI_Texture* tex_depth = rendererPath->GetShadowDepthTexture();
			RHI_Texture* tex_color = rendererPath->GetShadowColorTexture();
			if (!tex_depth)
				continue;

			// Define pipeline state
			static RHI_PipelineState pso;
			pso.blend_state = is_transparent_pass ? GetBlendState(Renderer_BlendState::Alpha).get() : GetBlendState(Renderer_BlendState::Off).get();
			pso.depth_stencil_state = is_transparent_pass ? GetDepthStencilState(Renderer_DepthStencilState::Read).get() : GetDepthStencilState(Renderer_DepthStencilState::ReadWrite).get();
			pso.render_target_color_textures[0] = tex_color; // always bind so we can clear to white (in case there are no transparent objects)
			pso.render_target_depth_texture = tex_depth;
			pso.primitive_topology = RHI_PrimitiveTopology::TriangleList;
			pso.name = "Pass_ShadowMaps";

			for (uint32_t array_index = 0; array_index < tex_depth->GetArrayLength(); array_index++)
			{
				// Set render target texture array index
				pso.render_target_color_texture_array_index = array_index;
				pso.render_target_depth_stencil_texture_array_index = array_index;

				// Set clear values
				pso.clear_color[0] = Color::standard_white;
				pso.clear_depth = is_transparent_pass ? rhi_depth_load : 0.0f; // reverse-z

				const Matrix& view_projection = rendererPath->GetLightViewMatrix(array_index) * rendererPath->GetLightProjectionMatrix(array_index);

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

				bool needBeginRenderPass = true;

				if(!entities.empty())
				{
					// draw non skin go
					for (GameObject* entity : entities)
					{
						if(SkinnedMeshRenderer* skinned_mesh_renderer = entity->GetComponent<SkinnedMeshRenderer>())
						{
							// draw has skin go
							pso.shader_vertex = shader_skin_v;
							pso.shader_pixel = shader_skin_p;
						}else
						{
							// Set pipeline state
							pso.shader_vertex = shader_v;
							pso.shader_pixel = shader_p;
						}

						cmd_list->SetPipelineState(pso, needBeginRenderPass);
						needBeginRenderPass = false;

						draw_renderable(cmd_list, pso, rendererPath, entity);
						// Set pass constants with cascade transform

						// m_cb_pass_cpu.set_f3_value2(static_cast<float>(array_index), static_cast<float>(light->GetIndex()), 0.0f);
						m_cb_pass_cpu.set_light(static_cast<float>(array_index), static_cast<float>(light_index), lightCount);
						m_cb_pass_cpu.transform = entity->GetComponent<Transform>()->GetMatrix() * view_projection;

						/*m_pcb_pass_cpu.set_f3_value(
							material->HasTexture(MaterialTexture::AlphaMask) ? 1.0f : 0.0f,
							material->HasTexture(MaterialTexture::Color) ? 1.0f : 0.0f
						);*/

						PushPassConstants(cmd_list);
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
		const vector<GameObject*>& entities = rendererables[is_transparent_pass ? Renderer_Entity::GeometryTransparent : Renderer_Entity::Geometry];
		if (entities.empty())
			return;


		cmd_list->BeginTimeblock(!is_transparent_pass ? "Pass_ForwardPass" : "Pass_ForwardPass_Transparent");
		
	
		// define PipelineState
		static RHI_PipelineState pso;
		pso.name = !is_transparent_pass ? "Pass_ForwardPass" : "Pass_ForwardPass_Transparent";
		//pso.shader_vertex = shader_v;
		//pso.shader_pixel = shader_p; // 
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Solid_cull_back).get();
		pso.blend_state = GetBlendState(Renderer_BlendState::Off).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::ReadWrite).get();
		// pso.render_target_depth_texture = GetRenderTarget(Renderer_RenderTarget::forward_pass_depth).get();// 不需要输出深度蒙版缓冲
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();// 不需要输出深度蒙版缓冲
		// pso.render_target_color_textures[0] = GetRenderTarget(Renderer_RenderTarget::frame_output).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.clear_depth = 0.0f; // reverse-z
		//pso.clear_color[0] = camera->GetClearColor();
		pso.primitive_topology = RHI_PrimitiveTopology::TriangleList;

		EASY_BLOCK("Render Entities")

		bool needBeginRenderPass = true;
		// draw non skin gameObject
		for (GameObject* entity : entities)
		{
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

			EASY_BLOCK("UpdateMaterial")
			UpdateMaterial(cmd_list, material);
			EASY_END_BLOCK

			// 如果是skinnedMesh 更新蒙皮数据
			if(skinnedMeshRenderer)
			{
				auto boneCbuffer = skinnedMeshRenderer->GetBoneConstantBuffer();
				cmd_list->SetConstantBuffer(Renderer_BindingsCb::boneArr, boneCbuffer);
			}

			if(rendererPath->GetMainLight()!=nullptr)
			{
				// just main light
				cmd_list->SetTexture(Renderer_BindingsSrv::light_directional_depth, rendererPath->GetShadowDepthTexture());
				m_cb_pass_cpu.set_light(static_cast<float>(0), static_cast<float>(0), rendererPath->GetLightCount());
				cmd_list->SetStructuredBuffer(Renderer_BindingsUav::sb_lights, rendererPath->GetLightBuffer());
				
			}

			EASY_BLOCK("PushPassConstants")
			m_cb_pass_cpu.set_resolution_out(rendererPath->GetDepthRenderTarget().get());
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

	void Renderer::Pass_Grid(RHI_CommandList* cmd_list, RendererPath* rendererPath)
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

	void Renderer::Pass_DebugGridPass(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		RHI_Shader* shader_v = GetShader(Renderer_Shader::line_v).get();
		RHI_Shader* shader_p = GetShader(Renderer_Shader::line_p).get();

		auto camera = rendererPath->GetRenderCamera();
		auto grid = GetStandardMesh(Renderer_MeshType::Grid);

		// define the pipeline state
		static RHI_PipelineState pso;
		pso.shader_vertex = shader_v;
		pso.shader_pixel = shader_p;
		pso.rasterizer_state = GetRasterizerState(Renderer_RasterizerState::Wireframe_cull_none).get();
		pso.render_target_color_textures[0] = rendererPath->GetColorRenderTarget().get();
		pso.clear_color[0] = rhi_color_load;
		pso.render_target_depth_texture = rendererPath->GetDepthRenderTarget().get();
		pso.primitive_topology = RHI_PrimitiveTopology::LineList;

		cmd_list->BeginMarker("DebugGridPass");

		// set pipeline state
		pso.blend_state = GetBlendState(Renderer_BlendState::Alpha).get();
		pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Read).get();

		cmd_list->SetPipelineState(pso,true);
		// push pass constants
		{
			m_cb_pass_cpu.set_resolution_out(GetResolutionRender());
			if (camera)
			{
				// To get the grid to feel infinite, it has to follow the camera,
				// but only by increments of the grid's spacing size. This gives the illusion 
				// that the grid never moves and if the grid is large enough, the user can't tell.
				const float grid_spacing = 1.0f;
				const Vector3 translation = Vector3
				(
					static_cast<int>(camera->GetPosition().x / grid_spacing) * grid_spacing,
					0.0f,
					static_cast<int>(camera->GetPosition().z / grid_spacing) * grid_spacing
				);

				m_cb_pass_cpu.transform = Matrix::CreateScale(grid_spacing) * Matrix::CreateTranslation(translation);
			}
			PushPassConstants(cmd_list);
		}
		cmd_list->SetBufferVertex(grid->GetVertexBuffer());
		cmd_list->Draw(grid->GetVertexCount());
		cmd_list->EndMarker();

	}

	void Renderer::Pass_SelectedAssetViewResourcePass(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		Material* selectMaterial = rendererPath->GetSelectedMaterial();
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

		// todo: 
		//UpdateDefaultConstantBufferLightArr(cmd_list, 1, rendererPath);

		cmd_list->BeginMarker("Pass_SelectedAssetViewResourcePass");

		// set pipeline state
		// pso.depth_stencil_state = GetDepthStencilState(Renderer_DepthStencilState::Depth_read).get();

		cmd_list->SetPipelineState(pso,true);
		// push pass constants
		{
			/*m_cb_pass_cpu.set_resolution_out(GetResolutionRender());*/
			EASY_BLOCK("PushPassConstants")
			// Set pass constants with cascade transform
			m_cb_pass_cpu.transform = transform;
			PushPassConstants(cmd_list);
			EASY_END_BLOCK
		}
		cmd_list->SetBufferVertex(m_vertex_buffer);
		cmd_list->SetBufferIndex(m_index_buffer);

		// 如果是skinnedMesh 更新蒙皮数据
		if (selectedResType == SelectedResourceType_Mesh && selectedMesh->IsAnimationModel())
		{
			cmd_list->SetConstantBuffer(Renderer_BindingsCb::boneArr, selectedMeshBoneConstantBuffer);
		}

		EASY_BLOCK("UpdateMaterial")
		UpdateMaterial(cmd_list, selectMaterial);
		EASY_END_BLOCK

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


	void Renderer::Pass_GenerateMips(RHI_CommandList* cmd_list, RHI_Texture* texture)
	{
	}


}
