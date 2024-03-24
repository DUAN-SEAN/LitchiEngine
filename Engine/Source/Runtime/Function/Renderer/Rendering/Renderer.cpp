
//= INCLUDES ===================================
#include "Runtime/Core/pch.h"
#include "Renderer.h"

#include <easy/profiler.h>
#include <easy/details/profiler_colors.h>

#include "../RHI/RHI_Device.h"
#include "../RHI/RHI_SwapChain.h"
#include "../RHI/RHI_RenderDoc.h"
#include "../RHI/RHI_CommandPool.h"
#include "../RHI/RHI_ConstantBuffer.h"
#include "../RHI/RHI_Implementation.h"
#include "../RHI/RHI_AMD_FidelityFX.h"
#include "../RHI/RHI_StructuredBuffer.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Core/Window/Window.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
//==============================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
	std::unordered_map<RendererPathType, RendererPath*> Renderer::m_rendererPaths;
	Pcb_Pass Renderer::m_cb_pass_cpu;
	Cb_Light Renderer::m_cb_light_cpu;
	Cb_Light_Arr Renderer::m_cb_light_arr_cpu;
	Cb_Material Renderer::m_cb_material_cpu;
	//RenderCamera* Renderer::m_main_camera;
	shared_ptr<RHI_VertexBuffer> Renderer::m_vertex_buffer_lines;
	unique_ptr<Font> Renderer::m_font;
	unique_ptr<Grid> Renderer::m_world_grid;
	vector<RHI_Vertex_PosCol> Renderer::m_line_vertices;
	vector<float> Renderer::m_lines_duration;
	uint32_t Renderer::m_lines_index_depth_off;
	uint32_t Renderer::m_lines_index_depth_on;
	bool Renderer::m_brdf_specular_lut_rendered;
	RHI_CommandPool* Renderer::m_cmd_pool = nullptr;

	namespace
	{
		bool is_game_mode = true;

		// states
		atomic<bool> is_rendering_allowed = true;
		atomic<bool> flush_requested = false;
		bool dirty_orthographic_projection = true;

		// resolution & viewport
		Vector2 m_resolution_render = Vector2::Zero;
		Vector2 m_resolution_output = Vector2::Zero;
		//RHI_Viewport m_viewport = RHI_Viewport(0, 0, 0, 0);

		// environment texture
		shared_ptr<RHI_Texture> environment_texture;
		mutex mutex_environment_texture;

		// swapchain
		const uint8_t swap_chain_buffer_count = 2;
		shared_ptr<RHI_SwapChain> swap_chain;

		// mip generation
		mutex mutex_mip_generation;
		vector<RHI_Texture*> textures_mip_generation;

		// rhi resources
		RHI_CommandList* cmd_current = nullptr;

		// misc
		array<float, 34> m_options;
		thread::id render_thread_id;
		mutex mutex_entity_addition;
		vector<GameObject*> m_entities_to_add;
		uint64_t frame_num = 0;
		Vector2 jitter_offset = Vector2::Zero;
		const uint32_t resolution_shadow_min = 128;
		float near_plane = 0.0f;
		float far_plane = 1.0f;
		uint32_t buffers_frames_since_last_reset = 0;

		void sort_renderables(RenderCamera* camera, vector<GameObject*>* renderables, const bool are_transparent)
		{
			if (!camera || renderables->size() <= 2)
				return;

			auto comparison_op = [camera](GameObject* entity)
			{
				auto renderable = entity->GetComponent<MeshFilter>();
				if (!renderable)
					return 0.0f;

				return (renderable->GetAAbb().GetCenter() - camera->GetPosition()).LengthSquared();
			};

			// sort by depth
			sort(renderables->begin(), renderables->end(), [&comparison_op, &are_transparent](GameObject* a, GameObject* b)
				{
					if (are_transparent)
					{
						return comparison_op(a) > comparison_op(b); // back-to-front for transparent
					}
					else
					{
						return comparison_op(a) < comparison_op(b); // front-to-back for opaque
					}
				});
		}
	}

	void Renderer::Initialize()
	{
		render_thread_id = this_thread::get_id();
		m_brdf_specular_lut_rendered = false;

		is_game_mode = ApplicationBase::Instance()->GetApplicationType() == ApplicationType::Game;

		// Display::DetectDisplayModes();

		// RHI initialization
		{
			RHI_Context::Initialize();

			/*if (RHI_Context::renderdoc)
			{
				RHI_RenderDoc::OnPreDeviceCreation();
			}*/

			RHI_Device::Initialize();
		}

		// resolution
		{
			auto& size = ApplicationBase::Instance()->window->GetSize();
			uint32_t width = size.first;
			uint32_t height = size.second;

			// The resolution of the actual rendering
			SetResolutionRender(width, height, false);

			// The resolution of the output frame *we can upscale to that linearly or with FSR 2)
			SetResolutionOutput(width, height, false);

			//// The resolution/size of the editor's viewport. This is overridden by the editor based on the actual viewport size
			//SetViewport(static_cast<float>(width), static_cast<float>(height));

			// Note: If the editor is active, it will set the render and viewport resolution to what the actual viewport is
		}

		// swap chain
		swap_chain = make_shared<RHI_SwapChain>
			(
				ApplicationBase::Instance()->window->GetGlfwWindow(),
				static_cast<uint32_t>(m_resolution_output.x),
				static_cast<uint32_t>(m_resolution_output.y),
				// Present mode: For v-sync, we could Mailbox for lower latency, but Fifo is always supported, so we'll assume that
				GetOption<bool>(Renderer_Option::Vsync) ? RHI_Present_Mode::Fifo : RHI_Present_Mode::Immediate,
				swap_chain_buffer_count,
				"renderer"
			);

		// command pool
		m_cmd_pool = RHI_Device::CommandPoolAllocate("renderer", swap_chain->GetObjectId(), RHI_Queue_Type::Graphics);

		//// AMD FidelityFX suite
		//RHI_AMD_FidelityFX::Initialize();

		// options
		m_options.fill(0.0f);
		SetOption(Renderer_Option::Hdr, swap_chain->IsHdr() ? 1.0f : 0.0f);                 // HDR is enabled by default if the swapchain is HDR
		SetOption(Renderer_Option::Bloom, 0.05f);                                             // non-zero values activate it and define the blend factor.
		SetOption(Renderer_Option::MotionBlur, 1.0f);
		SetOption(Renderer_Option::Ssgi, 1.0f);
		SetOption(Renderer_Option::ScreenSpaceShadows, 1.0f);
		SetOption(Renderer_Option::ScreenSpaceReflections, 1.0f);
		SetOption(Renderer_Option::Anisotropy, 16.0f);
		SetOption(Renderer_Option::ShadowResolution, 4096.0f);
		SetOption(Renderer_Option::Tonemapping, static_cast<float>(Renderer_Tonemapping::Disabled));
		SetOption(Renderer_Option::Gamma, 2.2f);
		SetOption(Renderer_Option::Exposure, 1.0f);
		SetOption(Renderer_Option::Sharpness, 0.5f);
		SetOption(Renderer_Option::FogDensity, 0.0f);
		SetOption(Renderer_Option::Antialiasing, static_cast<float>(Renderer_Antialiasing::TaaFxaa)); // this is using FSR 2 for TAA
		SetOption(Renderer_Option::Upsampling, static_cast<float>(Renderer_Upsampling::FSR2));
		SetOption(Renderer_Option::UpsamplingSharpness, 1.0f);
		SetOption(Renderer_Option::Vsync, 0.0f);
		SetOption(Renderer_Option::DepthPrepass, 0.0f);                                               // depth prepass is not always faster, so by default, it's disabled.
		SetOption(Renderer_Option::Debanding, 0.0f);
		SetOption(Renderer_Option::Debug_TransformHandle, 1.0f);
		SetOption(Renderer_Option::Debug_SelectionOutline, 1.0f);
		SetOption(Renderer_Option::Debug_Grid, 1.0f);
		SetOption(Renderer_Option::Debug_ReflectionProbes, 1.0f);
		SetOption(Renderer_Option::Debug_Lights, 1.0f);
		SetOption(Renderer_Option::Debug_Physics, 0.0f);
		SetOption(Renderer_Option::Debug_PerformanceMetrics, 1.0f);
		// Buggy or unused options
		//SetOption(Renderer_Option::PaperWhite,             150.0f);                                            // nits
		//SetOption(RendererOption::DepthOfField,            1.0f);                                              // this is depth of field from ALDI, so until I improve it, it should be disabled by default.
		//SetOption(RendererOption::VolumetricFog,           1.0f);                                              // disable by default because it's not that great, I need to do it with a voxelised approach.

		// resources
		CreateConstantBuffers();
		CreateShaders();
		CreateDepthStencilStates();
		CreateRasterizerStates();
		CreateBlendStates();
		CreateRenderTextures(true, true, true, true);
		CreateFonts();
		CreateSamplers(false);
		CreateStructuredBuffers();
		CreateStandardTextures();
		CreateStandardMeshes();// todo:

		m_rendererPaths.clear();
		for (size_t i = RendererPathType_Invalid + 1; i < RendererPathType_Count; i++)
		{
			m_rendererPaths[(RendererPathType)i] = nullptr;
		}

		// events
		{
			// todo
			//// subscribe
			//SP_SUBSCRIBE_TO_EVENT(EventType::WorldResolved,                   SP_EVENT_HANDLER_VARIANT_STATIC(OnWorldResolved));
			//SP_SUBSCRIBE_TO_EVENT(EventType::WorldClear,                      SP_EVENT_HANDLER_STATIC(OnClear));
			//SP_SUBSCRIBE_TO_EVENT(EventType::WindowFullscreenWindowedToggled, SP_EVENT_HANDLER_STATIC(OnFullScreenToggled));

			//// fire
			//SP_FIRE_EVENT(EventType::RendererOnInitialized);
		}


	}

	void Renderer::Shutdown()
	{
		// todo:
		//// console doesn't render anymore, log to file
		// Log::SetLogToFile(true);

		// todo:
		//// Fire event
		//SP_FIRE_EVENT(EventType::RendererOnShutdown);

		// Manually invoke the deconstructors so that ParseDeletionQueue(), releases their RHI resources.
		{
			DestroyResources();

			m_entities_to_add.clear();
			m_world_grid.reset();
			m_font.reset();
			swap_chain = nullptr;
			m_vertex_buffer_lines = nullptr;
			environment_texture = nullptr;
		}

		// RHI_RenderDoc::Shutdown();
		RHI_Device::QueueWaitAll();
		// RHI_AMD_FidelityFX::Destroy();
		RHI_Device::DeletionQueueParse();
		RHI_Device::Destroy();
	}

	void Renderer::Tick()
	{
		if (LitchiRuntime::frame_num == 1)
		{
			// Log::SetLogToFile(false);
			// SP_FIRE_EVENT(EventType::RendererOnFirstFrameCompleted); //TODO 第一帧后界面显示
		}

		EASY_BLOCK("Renderer::Flush")
		// happens when core resources are created/destroyed
		if (flush_requested)
		{
			Flush();
		}
		EASY_END_BLOCK;

		if (!is_rendering_allowed)
			return;

		EASY_BLOCK("Device::DeletionQueueParse")
		// delete any RHI resources that have accumulated
		if (RHI_Device::DeletionQueueNeedsToParse())
		{
			RHI_Device::QueueWaitAll();
			RHI_Device::DeletionQueueParse();
			DEBUG_LOG_INFO("Parsed deletion queue");
		}
		EASY_END_BLOCK;

		// reset buffer offsets
		{
			if (buffers_frames_since_last_reset == m_frames_in_flight)
			{
				for (shared_ptr<RHI_ConstantBuffer> constant_buffer : GetConstantBuffers())
				{
					constant_buffer->ResetOffset();
				}
				GetStructuredBuffer()->ResetOffset();

				buffers_frames_since_last_reset = true;
			}

			buffers_frames_since_last_reset++;
		}

		EASY_BLOCK("Device::Tick")
		RHI_Device::Tick(LitchiRuntime::frame_num);
		EASY_END_BLOCK

		EASY_BLOCK("Begin CommandList")
		// begin
		m_cmd_pool->Tick();
		cmd_current = m_cmd_pool->GetCurrentCommandList();
		cmd_current->Begin();
		EASY_END_BLOCK

		EASY_BLOCK("OnFrameStart")
		OnFrameStart(cmd_current);
		EASY_END_BLOCK

		// update frame constant buffer
		EASY_BLOCK("Build cb_frame")
		Cb_Frame frameBufferData = BuildFrameBufferData();
		UpdateConstantBufferFrame(cmd_current, frameBufferData, false);
		EASY_END_BLOCK

		EASY_BLOCK("Render4BuildInSceneView")
		// 绘制SceneView Path
		auto rendererPath4SceneView = m_rendererPaths[RendererPathType_SceneView];
		if (rendererPath4SceneView && rendererPath4SceneView->GetActive())
		{
			rendererPath4SceneView->UpdateRenderableGameObject();
			rendererPath4SceneView->UpdateLightShadow();

			// Render SceneView
			Render4BuildInSceneView(cmd_current, rendererPath4SceneView);

		}
		EASY_END_BLOCK


		EASY_BLOCK("Render4BuildInGameView")
		// 绘制SceneView Path
		auto rendererPath4GameView = m_rendererPaths[RendererPathType_GameView];
		if (rendererPath4GameView && rendererPath4GameView->GetActive())
		{
			// todo: only camera change need prepare camera
			rendererPath4GameView->UpdateRenderableGameObject();
			auto cameras = rendererPath4GameView->GetRenderables().at(Renderer_Entity::Camera);
			if (cameras.size() > 0)
			{
				// Get First Camera
				auto firstCamera = cameras[0]->GetComponent<Camera>()->GetRenderCamera();
				rendererPath4GameView->UpdateRenderCamera(firstCamera);
			}

			rendererPath4GameView->UpdateRenderableGameObject();
			rendererPath4GameView->UpdateLightShadow();

			// Render GameView
			Render4BuildInGameView(cmd_current, rendererPath4GameView);
		}
		EASY_END_BLOCK


		// blit to back buffer when in full screen
		if (is_game_mode)
		{
			cmd_current->BeginMarker("game_mode_copy_to_back_buffer");

			// todo temp code
			if(rendererPath4GameView!=nullptr)
			{
				auto colorTargetTexture = rendererPath4GameView->GetColorRenderTarget();
				cmd_current->Blit(colorTargetTexture.get(), swap_chain.get());
			}else
			{
				cmd_current->Blit(GetRenderTarget(Renderer_RenderTexture::frame_output).get(), swap_chain.get());
			}
			cmd_current->EndMarker();
		}

		OnFrameEnd(cmd_current);
		EASY_BLOCK("CommandList::Submit")
		// submit
		cmd_current->End();
		cmd_current->Submit();
		EASY_END_BLOCK

		// track frame
		LitchiRuntime::frame_num++;
	}

	void Renderer::Render4BuildInSceneView(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		auto camera = rendererPath->GetRenderCamera();
		auto rendererables = rendererPath->GetRenderables();
		auto canvas = rendererPath->GetCanvas();

		GetCmdList()->ClearRenderTarget(rendererPath->GetColorRenderTarget().get(),0, 0, false, camera->GetClearColor());

		// update rendererPath buffer
		EASY_BLOCK("Build cb_rendererPath")
		Cb_RendererPath rendererPathBufferData = BuildRendererPathFrameBufferData(camera, canvas);
		UpdateConstantBufferRenderPath(cmd_list, rendererPath, rendererPathBufferData);
		EASY_END_BLOCK

		//// update frame constant buffer
		//EASY_BLOCK("Build cb_frame")
		//Cb_Frame frameBufferData = BuildFrameBufferData();
		//UpdateConstantBufferFrame(cmd_list, frameBufferData, false);
		//EASY_END_BLOCK

		auto rt_output = rendererPath->GetColorRenderTarget().get();
		if (camera)
		{
			// determine if a transparent pass is required
			const bool do_transparent_pass = !rendererables[Renderer_Entity::GeometryTransparent].empty();

			// shadow maps
			{
				// todo: temp commit
				/*Pass_ShadowMaps(cmd_list, false);
				if (do_transparent_pass)
				{
					Pass_ShadowMaps(cmd_list, true);
				}*/
			}

			// opaque


			bool is_transparent_pass = false;
			EASY_BLOCK("Pass_ShadowMaps")
			Pass_ShadowMaps(cmd_list, rendererPath, is_transparent_pass);
			EASY_END_BLOCK

			EASY_BLOCK("Pass_ForwardPass")
			Pass_ForwardPass(cmd_list, rendererPath, is_transparent_pass);
			EASY_END_BLOCK

			EASY_BLOCK("Pass_ForwardPass")
			Pass_UIPass(cmd_list, rendererPath);
			EASY_END_BLOCK

			EASY_BLOCK("Pass_DebugGridPass")
			Pass_DebugGridPass(cmd_list, rendererPath);
			EASY_END_BLOCK


		}
		else
		{
			// if there is no camera, clear to black and and render the performance metrics
			GetCmdList()->ClearRenderTarget(rt_output, 0, 0, false, Color::standard_black);
		}

		// transition the render target to a readable state so it can be rendered
		// within the viewport or copied to the swap chain back buffer
		rt_output->SetLayout(RHI_Image_Layout::Shader_Read_Only_Optimal, cmd_list);
	}

	void Renderer::Render4BuildInGameView(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		auto camera = rendererPath->GetRenderCamera();
		auto canvas = rendererPath->GetCanvas();
		auto rendererables = rendererPath->GetRenderables();
		if (!camera)
		{
			return;
		}

		GetCmdList()->ClearRenderTarget(rendererPath->GetColorRenderTarget().get(), 0, 0, false, camera->GetClearColor());

		// update rendererPath buffer
		EASY_BLOCK("Build cb_rendererPath")
		Cb_RendererPath rendererPathBufferData = BuildRendererPathFrameBufferData(camera, canvas);
		UpdateConstantBufferRenderPath(cmd_list, rendererPath, rendererPathBufferData);
		EASY_END_BLOCK

		auto rt_output = rendererPath->GetColorRenderTarget().get();
		if (camera)
		{
			// determine if a transparent pass is required
			const bool do_transparent_pass = !rendererables[Renderer_Entity::GeometryTransparent].empty();
			
			// opaque
			bool is_transparent_pass = false;
			EASY_BLOCK("Pass_ShadowMaps")
			Pass_ShadowMaps(cmd_list, rendererPath, is_transparent_pass);
			EASY_END_BLOCK

			EASY_BLOCK("Pass_ForwardPass")
			Pass_ForwardPass(cmd_list, rendererPath, is_transparent_pass);
			EASY_END_BLOCK

			EASY_BLOCK("Pass_ForwardPass")
			Pass_UIPass(cmd_list, rendererPath);
			EASY_END_BLOCK
		}
		else
		{
			// if there is no camera, clear to black and and render the performance metrics
			GetCmdList()->ClearRenderTarget(rt_output, 0, 0, false, Color::standard_black);
		}

		// transition the render target to a readable state so it can be rendered
		// within the viewport or copied to the swap chain back buffer
		rt_output->SetLayout(RHI_Image_Layout::Shader_Read_Only_Optimal, cmd_list);
	}

	void Renderer::OnSceneResolved(std::vector<GameObject*> gameObjectList)
	{
		lock_guard lock(mutex_entity_addition);
		m_entities_to_add.clear();

		for (GameObject* entity : gameObjectList)
		{
			LC_ASSERT_MSG(entity != nullptr, "Entity is null");

			if (entity->GetActive())
			{
				m_entities_to_add.emplace_back(entity);
			}
		}
	}

	/*const RHI_Viewport& Renderer::GetViewport()
	{
		return m_viewport;
	}

	void Renderer::SetViewport(float width, float height)
	{
		SP_ASSERT_MSG(width != 0, "Width can't be zero");
		SP_ASSERT_MSG(height != 0, "Height can't be zero");

		if (m_viewport.width != width || m_viewport.height != height)
		{
			m_viewport.width = width;
			m_viewport.height = height;
			dirty_orthographic_projection = true;
		}
	}*/

	const Vector2& Renderer::GetResolutionRender()
	{
		return m_resolution_render;
	}

	void Renderer::SetResolutionRender(uint32_t width, uint32_t height, bool recreate_resources /*= true*/)
	{
		// Early exit if the resolution is invalid
		if (!RHI_Device::IsValidResolution(width, height))
		{
			DEBUG_LOG_WARN("%dx%d is an invalid resolution", width, height);
			return;
		}

		// Early exit if the resolution is already set
		if (m_resolution_render.x == width && m_resolution_render.y == height)
			return;

		// Set resolution
		m_resolution_render.x = static_cast<float>(width);
		m_resolution_render.y = static_cast<float>(height);

		if (recreate_resources)
		{
			// Re-create render textures
			CreateRenderTextures(true, false, false, true);

			// Re-create samplers
			CreateSamplers(true);
		}

		// todo:
		//// Register this resolution as a display mode so it shows up in the editor's render options (it won't happen if already registered)
		//Display::RegisterDisplayMode(static_cast<uint32_t>(width), static_cast<uint32_t>(height), Display::GetRefreshRate(), Display::GetIndex());

		// Log
		DEBUG_LOG_INFO("Render resolution has been set to {}x{}", width, height);
	}

	const Vector2& Renderer::GetResolutionOutput()
	{
		return m_resolution_output;
	}

	void Renderer::SetResolutionOutput(uint32_t width, uint32_t height, bool recreate_resources /*= true*/)
	{
		// Return if resolution is invalid
		if (!RHI_Device::IsValidResolution(width, height))
		{
			DEBUG_LOG_WARN("{}x{} is an invalid resolution", width, height);
			return;
		}

		// Silently return if resolution is already set
		if (m_resolution_output.x == width && m_resolution_output.y == height)
			return;

		// Set resolution
		m_resolution_output.x = static_cast<float>(width);
		m_resolution_output.y = static_cast<float>(height);

		if (recreate_resources)
		{
			// Re-create render textures
			CreateRenderTextures(false, true, false, true);

			// Re-create samplers
			CreateSamplers(true);
		}

		// Log
		DEBUG_LOG_INFO("Output resolution output has been set to %dx%d", width, height);
	}

	void Renderer::UpdateConstantBufferFrame(RHI_CommandList* cmd_list,Cb_Frame& frameBufferData, const bool set /*= true*/)
	{
		GetConstantBuffer(Renderer_ConstantBuffer::Frame)->Update(&frameBufferData);

		// Bind because the offset just changed
		if (set)
		{
			cmd_list->SetConstantBuffer(Renderer_BindingsCb::frame, GetConstantBuffer(Renderer_ConstantBuffer::Frame));
		}
	}

	void Renderer::PushPassConstants(RHI_CommandList* cmd_list)
	{
		cmd_list->PushConstants(0, sizeof(Pcb_Pass), &m_cb_pass_cpu);
	}

	void Renderer::UpdateConstantBufferLight(RHI_CommandList* cmd_list, Light* light, RendererPath* rendererPath)
	{
		for (uint32_t i = 0; i < rendererPath->GetShadowArraySize(); i++)
		{
			m_cb_light_cpu.view_projection[i] = rendererPath->GetLightViewMatrix(i) * rendererPath->GetLightProjectionMatrix(i);
		}

		m_cb_light_cpu.intensity_range_angle_bias = Vector4
		(
			light->GetIntensityWatt(rendererPath->GetRenderCamera()),
			light->GetRange(), light->GetAngle(),
			light->GetBias()
		);

		m_cb_light_cpu.color = light->GetColor();
		m_cb_light_cpu.normal_bias = light->GetNormalBias();
		m_cb_light_cpu.position = light->GetGameObject()->GetComponent<Transform>()->GetPosition();
		m_cb_light_cpu.direction = light->GetGameObject()->GetComponent<Transform>()->GetForward();
		m_cb_light_cpu.options = 0;
		m_cb_light_cpu.options |= light->GetLightType() == LightType::Directional ? (1 << 0) : 0;
		m_cb_light_cpu.options |= light->GetLightType() == LightType::Point ? (1 << 1) : 0;
		m_cb_light_cpu.options |= light->GetLightType() == LightType::Spot ? (1 << 2) : 0;
		m_cb_light_cpu.options |= light->GetShadowsEnabled() ? (1 << 3) : 0;
		m_cb_light_cpu.options |= light->GetShadowsTransparentEnabled() ? (1 << 4) : 0;
		m_cb_light_cpu.options |= light->GetVolumetricEnabled() ? (1 << 5) : 0;

		GetConstantBuffer(Renderer_ConstantBuffer::Light)->Update(&m_cb_light_cpu);

		// Bind because the offset just changed
		cmd_list->SetConstantBuffer(Renderer_BindingsCb::light, GetConstantBuffer(Renderer_ConstantBuffer::Light));
	}

	void Renderer::UpdateConstantBufferLightArr(RHI_CommandList* cmd_list, Light** lightArr, const int lightCount, RendererPath* rendererPath)
	{
		// GetConstantBuffer(Renderer_ConstantBuffer::LightArr)->ResetOffset();

		m_cb_light_arr_cpu.lightCount = lightCount;

		for (int index = 0; index < lightCount; index++)
		{
			const auto light = lightArr[index];

			// todo only one light has shadow, is temp code
			for (uint32_t i = 0; i < rendererPath->GetShadowArraySize(); i++)
			{
				m_cb_light_arr_cpu.lightArr[index].view_projection[i] = rendererPath->GetLightViewMatrix(i) * rendererPath->GetLightProjectionMatrix(i);
			}

			m_cb_light_arr_cpu.lightArr[index].intensity_range_angle_bias = Vector4
			(
				light->GetIntensityWatt(rendererPath->GetRenderCamera()),
				light->GetRange(), light->GetAngle(),
				light->GetBias()
			);

			m_cb_light_arr_cpu.lightArr[index].color = light->GetColor();
			m_cb_light_arr_cpu.lightArr[index].normal_bias = light->GetNormalBias();
			m_cb_light_arr_cpu.lightArr[index].position = light->GetGameObject()->GetComponent<Transform>()->GetPosition();
			m_cb_light_arr_cpu.lightArr[index].direction = light->GetGameObject()->GetComponent<Transform>()->GetForward();
			m_cb_light_arr_cpu.lightArr[index].options = 0;
			m_cb_light_arr_cpu.lightArr[index].options |= light->GetLightType() == LightType::Directional ? (1 << 0) : 0;
			m_cb_light_arr_cpu.lightArr[index].options |= light->GetLightType() == LightType::Point ? (1 << 1) : 0;
			m_cb_light_arr_cpu.lightArr[index].options |= light->GetLightType() == LightType::Spot ? (1 << 2) : 0;
			m_cb_light_arr_cpu.lightArr[index].options |= light->GetShadowsEnabled() ? (1 << 3) : 0;
			m_cb_light_arr_cpu.lightArr[index].options |= light->GetShadowsTransparentEnabled() ? (1 << 4) : 0;
			m_cb_light_arr_cpu.lightArr[index].options |= light->GetVolumetricEnabled() ? (1 << 5) : 0;
		}

		GetConstantBuffer(Renderer_ConstantBuffer::LightArr)->Update(&m_cb_light_arr_cpu);
	}

	void Renderer::UpdateConstantBufferMaterial(RHI_CommandList* cmd_list, Material* material)
	{
		// Set
		/*m_cb_material_cpu.color.x = material->GetProperty(MaterialProperty::ColorR);
		m_cb_material_cpu.color.y = material->GetProperty(MaterialProperty::ColorG);
		m_cb_material_cpu.color.z = material->GetProperty(MaterialProperty::ColorB);
		m_cb_material_cpu.color.w = material->GetProperty(MaterialProperty::ColorA);
		m_cb_material_cpu.tiling_uv.x = material->GetProperty(MaterialProperty::UvTilingX);
		m_cb_material_cpu.tiling_uv.y = material->GetProperty(MaterialProperty::UvTilingY);
		m_cb_material_cpu.offset_uv.x = material->GetProperty(MaterialProperty::UvOffsetX);
		m_cb_material_cpu.offset_uv.y = material->GetProperty(MaterialProperty::UvOffsetY);
		m_cb_material_cpu.roughness_mul = material->GetProperty(MaterialProperty::RoughnessMultiplier);
		m_cb_material_cpu.metallic_mul = material->GetProperty(MaterialProperty::MetalnessMultiplier);
		m_cb_material_cpu.normal_mul = material->GetProperty(MaterialProperty::NormalMultiplier);
		m_cb_material_cpu.height_mul = material->GetProperty(MaterialProperty::HeightMultiplier);
		m_cb_material_cpu.anisotropic = material->GetProperty(MaterialProperty::Anisotropic);
		m_cb_material_cpu.anisitropic_rotation = material->GetProperty(MaterialProperty::AnisotropicRotation);
		m_cb_material_cpu.clearcoat = material->GetProperty(MaterialProperty::Clearcoat);
		m_cb_material_cpu.clearcoat_roughness = material->GetProperty(MaterialProperty::Clearcoat_Roughness);
		m_cb_material_cpu.sheen = material->GetProperty(MaterialProperty::Sheen);
		m_cb_material_cpu.sheen_tint = material->GetProperty(MaterialProperty::SheenTint);
		m_cb_material_cpu.properties = 0;
		m_cb_material_cpu.properties |= material->GetProperty(MaterialProperty::SingleTextureRoughnessMetalness) ? (1U << 0) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::Height) ? (1U << 1) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::Normal) ? (1U << 2) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::Color) ? (1U << 3) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::Roughness) ? (1U << 4) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::Metalness) ? (1U << 5) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::AlphaMask) ? (1U << 6) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::Emission) ? (1U << 7) : 0;
		m_cb_material_cpu.properties |= material->HasTexture(MaterialTexture::Occlusion) ? (1U << 8) : 0;*/

		// Update
		GetConstantBuffer(Renderer_ConstantBuffer::Material)->Update(&m_cb_material_cpu);

		// Bind because the offset just changed
		cmd_list->SetConstantBuffer(Renderer_BindingsCb::material, GetConstantBuffer(Renderer_ConstantBuffer::Material));
	}

	void Renderer::UpdateMaterial(RHI_CommandList* cmd_list, Material* material)
	{
		EASY_BLOCK("SetMaterialGlobalBuffer")
			// todo: if material state change, maybe not update to shader !
			uint32_t size;
		auto cbuffer = material->GetValuesCBuffer().get();
		cbuffer->UpdateWithReset(material->GetValues4DescriptorSet(size));
		cmd_list->SetMaterialGlobalBuffer(material->GetValuesCBuffer().get());
		EASY_END_BLOCK

		EASY_BLOCK("SetTextures")
		auto textureMap = material->GetTextures4DescriptorSet();
		for (auto texture_map : textureMap)
		{
			cmd_list->SetTexture(texture_map.first, texture_map.second);
		}
		EASY_END_BLOCK
	}

	void Renderer::UpdateConstantBufferRenderPath(RHI_CommandList* cmd_list,RendererPath* rendererPath, Cb_RendererPath& renderPathBufferData)
	{
		// set buffer
		GetConstantBuffer(Renderer_ConstantBuffer::RendererPath)->Update(&renderPathBufferData);

		//auto constantBuffer = rendererPath->GetConstantBuffer();
		//// update cb
		//constantBuffer->UpdateWithReset(static_cast<void*>(&renderPathBufferData));
		//// set buffer
		//cmd_list->SetConstantBuffer(Renderer_BindingsCb::rendererPath, constantBuffer);
	}

	// todo: 
	//void Renderer::OnWorldResolved(sp_variant data)
	//{
	//    // note: m_renderables is a vector of shared pointers.
	//    // this ensures that if any entities are deallocated by the world.
	//    // we'll still have some valid pointers until the are overridden by m_renderables_world.

	//    vector<shared_ptr<Entity>> entities = get<vector<shared_ptr<Entity>>>(data);

	//    lock_guard lock(mutex_entity_addition);
	//    m_entities_to_add.clear();

	//    for (shared_ptr<Entity> entity : entities)
	//    {
	//        SP_ASSERT_MSG(entity != nullptr, "Entity is null");

	//        if (entity->IsActiveRecursively())
	//        {
	//            m_entities_to_add.emplace_back(entity);
	//        }
	//    }
	//}

	void Renderer::OnClear()
	{
		// Flush to remove references to entity resources that will be deallocated
		Flush();
	}

	// todo: remove
	//void Renderer::OnFullScreenToggled()
	//{
	//	static float    width_previous_viewport = 0;
	//	static float    height_previous_viewport = 0;
	//	static uint32_t width_previous_output = 0;
	//	static uint32_t height_previous_output = 0;

	//	if (ApplicationBase::Instance()->window->IsFullscreen())
	//	{
	//		uint32_t width = ApplicationBase::Instance()->window->GetWidth();
	//		uint32_t height = ApplicationBase::Instance()->window->GetHeight();

	//		width_previous_viewport = m_viewport.width;
	//		height_previous_viewport = m_viewport.height;
	//		SetViewport(static_cast<float>(width), static_cast<float>(height));

	//		width_previous_output = static_cast<uint32_t>(m_viewport.width);
	//		height_previous_output = static_cast<uint32_t>(m_viewport.height);
	//		SetResolutionOutput(width, height);
	//	}
	//	else
	//	{
	//		SetViewport(width_previous_viewport, height_previous_viewport);
	//		SetResolutionOutput(width_previous_output, height_previous_output);
	//	}

	//	//  InputManager::SetMouseCursorVisible(!ApplicationBase::Instance()->window->IsFullscreen());
	//}

	void Renderer::OnFrameStart(RHI_CommandList* cmd_list)
	{
		// 全量更新

		// generate mips
		{
			lock_guard lock(mutex_mip_generation);
			for (RHI_Texture* texture : textures_mip_generation)
			{
				Pass_GenerateMips(cmd_list, texture);
			}
			textures_mip_generation.clear();
		}

		// Lines_OneFrameStart();
	}

	void Renderer::OnFrameEnd(RHI_CommandList* cmd_list)
	{
		// Lines_OnFrameEnd();
	}

	bool Renderer::IsCallingFromOtherThread()
	{
		return render_thread_id != this_thread::get_id();
	}

	const shared_ptr<RHI_Texture> Renderer::GetEnvironmentTexture()
	{
		return environment_texture ? environment_texture : GetStandardTexture(Renderer_StandardTexture::Black);
	}

	void Renderer::SetEnvironment(Environment* environment)
	{
		// todo:
		/*lock_guard lock(mutex_environment_texture);
		environment_texture = environment->GetTexture();*/
	}

	void Renderer::SetOption(Renderer_Option option, float value)
	{
		// Clamp value
		{
			// Anisotropy
			if (option == Renderer_Option::Anisotropy)
			{
				value = Helper::Clamp(value, 0.0f, 16.0f);
			}
			// Shadow resolution
			else if (option == Renderer_Option::ShadowResolution)
			{
				value = Helper::Clamp(value, static_cast<float>(resolution_shadow_min), static_cast<float>(RHI_Device::PropertyGetMaxTexture2dDimension()));
			}
		}

		// Early exit if the value is already set
		if (m_options[static_cast<uint32_t>(option)] == value)
			return;

		// todo:
		//// Reject changes (if needed)
		//{
		//    if (option == Renderer_Option::Hdr)
		//    {
		//        if (value == 1.0f && !Display::GetHdr())
		//        {
		//            DEBUG_LOG_INFO("This display doesn't support HDR");
		//            return;
		//        }
		//    }
		//}

		// Set new value
		m_options[static_cast<uint32_t>(option)] = value;

		// Handle cascading changes
		{
			// Antialiasing
			if (option == Renderer_Option::Antialiasing)
			{
				bool taa_enabled = value == static_cast<float>(Renderer_Antialiasing::Taa) || value == static_cast<float>(Renderer_Antialiasing::TaaFxaa);
				bool fsr_enabled = GetOption<Renderer_Upsampling>(Renderer_Option::Upsampling) == Renderer_Upsampling::FSR2;

				if (taa_enabled)
				{
					// Implicitly enable FSR since it's doing TAA.
					if (!fsr_enabled)
					{
						m_options[static_cast<uint32_t>(Renderer_Option::Upsampling)] = static_cast<float>(Renderer_Upsampling::FSR2);
						// RHI_AMD_FidelityFX::FSR2_ResetHistory();
						DEBUG_LOG_INFO("Enabled FSR 2.0 since it's used for TAA.");
					}
				}
				else
				{
					// Implicitly disable FSR since it's doing TAA
					if (fsr_enabled)
					{
						m_options[static_cast<uint32_t>(Renderer_Option::Upsampling)] = static_cast<float>(Renderer_Upsampling::Linear);
						DEBUG_LOG_INFO("Disabed FSR 2.0 since it's used for TAA.");
					}
				}
			}
			// Upsampling
			else if (option == Renderer_Option::Upsampling)
			{
				bool taa_enabled = GetOption<Renderer_Antialiasing>(Renderer_Option::Antialiasing) == Renderer_Antialiasing::Taa;

				if (value == static_cast<float>(Renderer_Upsampling::Linear))
				{
					// Implicitly disable TAA since FSR 2.0 is doing it
					if (taa_enabled)
					{
						m_options[static_cast<uint32_t>(Renderer_Option::Antialiasing)] = static_cast<float>(Renderer_Antialiasing::Disabled);
						DEBUG_LOG_INFO("Disabled TAA since it's done by FSR 2.0.");
					}
				}
				else if (value == static_cast<float>(Renderer_Upsampling::FSR2))
				{
					// Implicitly enable TAA since FSR 2.0 is doing it
					if (!taa_enabled)
					{
						m_options[static_cast<uint32_t>(Renderer_Option::Antialiasing)] = static_cast<float>(Renderer_Antialiasing::Taa);
						// RHI_AMD_FidelityFX::FSR2_ResetHistory();
						DEBUG_LOG_INFO("Enabled TAA since FSR 2.0 does it.");
					}
				}
			}
			//// Shadow resolution
			//else if (option == Renderer_Option::ShadowResolution)
			//{
			//	const auto& light_entities = m_renderables[Renderer_Entity::Light];
			//	for (const auto& light_entity : light_entities)
			//	{
			//		auto light = light_entity->GetComponent<Light>();
			//		if (light->GetShadowsEnabled())
			//		{
			//			light->CreateShadowMap();
			//		}
			//	}
			//}
			else if (option == Renderer_Option::Hdr)
			{
				swap_chain->SetHdr(value == 1.0f);
			}
			else if (option == Renderer_Option::Vsync)
			{
				swap_chain->SetVsync(value == 1.0f);
			}
		}
	}

	array<float, 34>& Renderer::GetOptions()
	{
		return m_options;
	}

	void Renderer::SetOptions(array<float, 34> options)
	{
		m_options = options;
	}

	RHI_SwapChain* Renderer::GetSwapChain()
	{
		return swap_chain.get();
	}

	void Renderer::Present()
	{
		if (!is_rendering_allowed)
			return;

		LC_ASSERT_MSG(!ApplicationBase::Instance()->window->IsMinimized(), "Don't call present if the window is minimized");
		LC_ASSERT(swap_chain->GetLayout() == RHI_Image_Layout::Present_Src);

		swap_chain->Present();

		// SP_FIRE_EVENT(EventType::RendererPostPresent);
	}

	void Renderer::Flush()
	{
		// The external thread requests a flush from the renderer thread (to avoid a myriad of thread issues and Vulkan errors)
		if (IsCallingFromOtherThread())
		{
			is_rendering_allowed = false;
			flush_requested = true;

			while (flush_requested)
			{
				DEBUG_LOG_INFO("External thread is waiting for the renderer thread to flush...");
				this_thread::sleep_for(chrono::milliseconds(16));
			}

			return;
		}

		// Flushing
		if (!is_rendering_allowed)
		{
			DEBUG_LOG_INFO("Renderer thread is flushing...");
			RHI_Device::QueueWaitAll();
		}

		flush_requested = false;
	}

	void Renderer::AddTextureForMipGeneration(RHI_Texture* texture)
	{
		lock_guard<mutex> guard(mutex_mip_generation);
		textures_mip_generation.push_back(texture);
	}

	void Renderer::Pass_GenerateMips(RHI_CommandList* cmd_list, RHI_Texture* texture)
	{
	}

	RHI_CommandList* Renderer::GetCmdList()
	{
		return cmd_current;
	}

	RHI_Api_Type Renderer::GetRhiApiType()
	{
		return RHI_Context::api_type;
	}

	RHI_Texture* Renderer::GetFrameTexture()
	{
		return GetRenderTarget(Renderer_RenderTexture::frame_output).get();
	}

	uint64_t Renderer::GetFrameNum()
	{
		return frame_num;
	}

	void Renderer::UpdateRendererPath(RendererPathType rendererPathType, RendererPath* rendererPath)
	{
		m_rendererPaths[rendererPathType] = rendererPath;
	}


	Cb_Frame Renderer::BuildFrameBufferData()
	{
		Cb_Frame cb_frame_cpu;
		cb_frame_cpu.resolution_output = m_resolution_output;
		cb_frame_cpu.resolution_render = m_resolution_render;
		cb_frame_cpu.taa_jitter_previous = cb_frame_cpu.taa_jitter_current;
		cb_frame_cpu.taa_jitter_current = jitter_offset;
		cb_frame_cpu.delta_time = static_cast<float>(Time::delta_time());
		cb_frame_cpu.gamma = GetOption<float>(Renderer_Option::Gamma);
		cb_frame_cpu.frame = static_cast<uint32_t>(LitchiRuntime::frame_num);

		// These must match what Common_Buffer.hlsl is reading
		cb_frame_cpu.set_bit(GetOption<bool>(Renderer_Option::ScreenSpaceReflections), 1 << 0);
		cb_frame_cpu.set_bit(GetOption<bool>(Renderer_Option::Ssgi), 1 << 1);
		cb_frame_cpu.set_bit(GetOption<bool>(Renderer_Option::VolumetricFog), 1 << 2);
		cb_frame_cpu.set_bit(GetOption<bool>(Renderer_Option::ScreenSpaceShadows), 1 << 3);
		return cb_frame_cpu;
	}

	Cb_RendererPath Renderer::BuildRendererPathFrameBufferData(RenderCamera* camera,UICanvas* canvas)
	{
		Cb_RendererPath rendererPathBufferData{};
		if (camera)
		{
			if (near_plane != camera->GetNearPlane() || far_plane != camera->GetFarPlane())
			{
				near_plane = camera->GetNearPlane();
				far_plane = camera->GetFarPlane();
				dirty_orthographic_projection = true;
			}

			rendererPathBufferData.view = camera->GetViewMatrix();
			rendererPathBufferData.projection = camera->GetProjectionMatrix();
		}
		
		if (canvas!=nullptr)
		{
			// if (dirty_orthographic_projection)
			{
				float canvasResolutionWidth = canvas->GetResolution().x;
				float canvasResolutionHeight = canvas->GetResolution().y;

				// near clip does not affect depth accuracy in orthographic projection, so set it to 0 to avoid problems which can result an infinitely small [3,2] (NaN) after the multiplication below.
				Matrix projection_ortho = Matrix::CreateOrthographicLH(canvasResolutionWidth, canvasResolutionHeight, 0.0f, far_plane);
				rendererPathBufferData.view_projection_ortho = Matrix::CreateLookAtLH(Vector3(0, 0, -near_plane), Vector3::Forward, Vector3::Up) * projection_ortho;
				dirty_orthographic_projection = false;
			}

		}

		// update the remaining of the frame buffer
		rendererPathBufferData.view_projection_previous = rendererPathBufferData.view_projection;
		rendererPathBufferData.view_projection = rendererPathBufferData.view * rendererPathBufferData.projection;
		rendererPathBufferData.view_projection_inv = Matrix::Invert(rendererPathBufferData.view_projection);
		if (camera)
		{
			rendererPathBufferData.view_projection_unjittered = rendererPathBufferData.view * camera->GetProjectionMatrix();
			rendererPathBufferData.camera_near = camera->GetNearPlane();
			rendererPathBufferData.camera_far = camera->GetFarPlane();
			rendererPathBufferData.camera_position = camera->GetPosition();
			rendererPathBufferData.camera_direction = camera->GetForward();
		}

		return rendererPathBufferData;
	}


	/*RenderCamera* Renderer::GetMainCamera()
	{
		return m_main_camera;
	}*/
}
