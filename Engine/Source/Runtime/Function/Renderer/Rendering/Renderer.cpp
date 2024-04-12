
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
	Pcb_Pass Renderer::m_cb_pass_cpu;
	Cb_Light Renderer::m_cb_light_cpu;
	Cb_Light_Arr Renderer::m_cb_light_arr_cpu;

	RHI_CommandPool* Renderer::m_cmd_pool = nullptr;
	uint32_t Renderer::m_resource_index = 0;

	std::unordered_map<RendererPathType, RendererPath*> Renderer::m_rendererPaths;

	bool Renderer::m_brdf_specular_lut_rendered;

	Material* Renderer::m_default_standard_material;
	Material* Renderer::m_default_standard_skin_material;
	shared_ptr<RHI_VertexBuffer> Renderer::m_vertex_buffer_skyBox;
	shared_ptr<RHI_IndexBuffer> Renderer::m_index_buffer_skyBox;

	// line rendering
	shared_ptr<RHI_VertexBuffer> Renderer::m_vertex_buffer_lines;
	vector<RHI_Vertex_PosCol> Renderer::m_line_vertices;
	vector<float> Renderer::m_lines_duration;
	uint32_t Renderer::m_lines_index_depth_off;
	uint32_t Renderer::m_lines_index_depth_on;

	namespace
	{
		bool is_standalone = true;

		// resolution & viewport
		Vector2 m_resolution_render = Vector2::Zero;
		Vector2 m_resolution_output = Vector2::Zero;
		//RHI_Viewport m_viewport = RHI_Viewport(0, 0, 0, 0);

		// swapchain
		const uint8_t swap_chain_buffer_count = 2;
		shared_ptr<RHI_SwapChain> swap_chain;

		// mip generation
		mutex mutex_mip_generation;
		vector<RHI_Texture*> textures_mip_generation;

		// rhi resources
		RHI_CommandList* cmd_current = nullptr;

		// misc
		unordered_map<Renderer_Option, float> m_options;
		uint64_t frame_num = 0;
		Vector2 jitter_offset = Vector2::Zero;
		const uint32_t resolution_shadow_min = 128;
		float near_plane = 0.0f;
		float far_plane = 1.0f;

	}

	void Renderer::Initialize()
	{
		m_brdf_specular_lut_rendered = false;

		is_standalone = ApplicationBase::Instance()->GetApplicationType() == LitchiApplicationType::Game;

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

			// The resolution of the output frame *we can upscale to that linearly or with FSR 2)
			SetResolutionOutput(width, height, false);

			// The resolution of the actual rendering
			SetResolutionRender(width, height, false);

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
				false,// temp
				"renderer"
			);

		// command pool
		m_cmd_pool = RHI_Device::CommandPoolAllocate("renderer", swap_chain->GetObjectId(), RHI_Queue_Type::Graphics);

		//// AMD FidelityFX suite
		//RHI_AMD_FidelityFX::Initialize();

		  // options
		m_options.clear();
		SetOption(Renderer_Option::Hdr, swap_chain->IsHdr() ? 1.0f : 0.0f);
		SetOption(Renderer_Option::WhitePoint, 350.0f);
		//SetOption(Renderer_Option::Tonemapping, static_cast<float>(Renderer_Tonemapping::Max));
		SetOption(Renderer_Option::Bloom, 0.03f);                                                // non-zero values activate it and define the blend factor
		SetOption(Renderer_Option::MotionBlur, 1.0f);
		SetOption(Renderer_Option::ScreenSpaceGlobalIllumination, 1.0f);
		//SetOption(Renderer_Option::ScreenSpaceShadows, static_cast<float>(Renderer_ScreenspaceShadow::Bend));
		SetOption(Renderer_Option::ScreenSpaceReflections, 1.0f);
		SetOption(Renderer_Option::Anisotropy, 16.0f);
		SetOption(Renderer_Option::ShadowResolution, 2048.0f);
		SetOption(Renderer_Option::Exposure, 1.0f);
		SetOption(Renderer_Option::Sharpness, 0.5f);                                                 // becomes the upsampler's sharpness as well
		SetOption(Renderer_Option::Fog, 0.3f);                                                 // controls the intensity of the volumetric fog as well
		SetOption(Renderer_Option::FogVolumetric, 1.0f);                                                 // these is only a toggle for the volumetric fog
		SetOption(Renderer_Option::Antialiasing, static_cast<float>(Renderer_Antialiasing::Taa));       // this is using fsr 2 for taa
		SetOption(Renderer_Option::Upsampling, static_cast<float>(Renderer_Upsampling::Fsr2));
		SetOption(Renderer_Option::ResolutionScale, 1.0f);
		SetOption(Renderer_Option::VariableRateShading, 0.0f);
		SetOption(Renderer_Option::Vsync, 0.0f);
		SetOption(Renderer_Option::TransformHandle, 1.0f);
		SetOption(Renderer_Option::SelectionOutline, 1.0f);
		SetOption(Renderer_Option::Grid, 1.0f);
		SetOption(Renderer_Option::Lights, 1.0f);
		SetOption(Renderer_Option::Physics, 0.0f);
		SetOption(Renderer_Option::PerformanceMetrics, 1.0f);

		// resources
		CreateConstantBuffers();
		CreateShaders();
		CreateDepthStencilStates();
		CreateRasterizerStates();
		CreateBlendStates();
		CreateRenderTargets(true, true, true);
		CreateFonts();
		CreateSamplers(false);
		CreateStructuredBuffers();
		CreateStandardTextures();
		CreateStandardMeshes();// todo:
		LoadDefaultMaterials();
		CreateSkyBoxMesh();

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
		// Manually invoke the deconstructors so that ParseDeletionQueue(), releases their RHI resources.
		{
			DestroyResources();

			m_default_standard_material = nullptr;// unload by resourceManager
			swap_chain = nullptr;
			m_vertex_buffer_lines = nullptr;
		}

		// RHI_RenderDoc::Shutdown();
		RHI_Device::QueueWaitAll();
		// RHI_AMD_FidelityFX::Destroy();
		RHI_Device::Destroy();
	}

	void Renderer::Tick()
	{
		if (LitchiRuntime::frame_num == 1)
		{
			// Log::SetLogToFile(false);
			// SP_FIRE_EVENT(EventType::RendererOnFirstFrameCompleted); //TODO 第一帧后界面显示
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

		OnSyncPoint(cmd_current);

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

		EASY_BLOCK("Render4BuildInAssetView")
		// 绘制SceneView Path
		auto rendererPath4AssetView = m_rendererPaths[RendererPathType_AssetView];
		if (rendererPath4AssetView && rendererPath4AssetView->GetActive())
		{
			rendererPath4AssetView->UpdateRenderableGameObject();
			rendererPath4AssetView->UpdateLightShadow();

			// Render SceneView
			Render4BuildInAssetView(cmd_current, rendererPath4AssetView);

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
		if (is_standalone)
		{
			// todo temp code
			if(rendererPath4GameView!=nullptr)
			{
				auto colorTargetTexture = rendererPath4GameView->GetColorRenderTarget();

				BlitToBackBuffer(cmd_current, colorTargetTexture.get());
			}else
			{
				DEBUG_LOG_ERROR("GameViewRendererPath is Null");
			}

		}

		EASY_BLOCK("CommandList::Submit")
		// submit
		cmd_current->End();
		cmd_current->Submit();
		EASY_END_BLOCK

		// present
		if (is_standalone)
		{
			swap_chain->Present();
		}

		// track frame
		LitchiRuntime::frame_num++;
	}

	void Renderer::Render4BuildInSceneView(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		auto camera = rendererPath->GetRenderCamera();
		auto rendererables = rendererPath->GetRenderables();
		auto canvas = rendererPath->GetCanvas();
		auto scene = rendererPath->GetRenderScene();

		GetCmdList()->ClearRenderTarget(rendererPath->GetColorRenderTarget().get(),0, 0, false, camera->GetClearColor());

		// update rendererPath buffer
		EASY_BLOCK("Build cb_rendererPath")
		Cb_RendererPath rendererPathBufferData = BuildRendererPathFrameBufferData(camera, canvas);
		UpdateConstantBufferRenderPath(cmd_list, rendererPath, rendererPathBufferData);
		EASY_END_BLOCK

		auto rt_output = rendererPath->GetColorRenderTarget().get();
		if (camera && scene)
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
			EASY_BLOCK("Pass_SkyBox")
			Pass_SkyBox(cmd_list, rendererPath);
			EASY_END_BLOCK

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
		rt_output->SetLayout(RHI_Image_Layout::Shader_Read, cmd_list);
	}

	void Renderer::Render4BuildInAssetView(RHI_CommandList* cmd_list, RendererPath* rendererPath)
	{
		auto camera = rendererPath->GetRenderCamera();

		GetCmdList()->ClearRenderTarget(rendererPath->GetColorRenderTarget().get(), 0, 0, false, camera->GetClearColor());

		// update rendererPath buffer
		EASY_BLOCK("Build cb_rendererPath")
		Cb_RendererPath rendererPathBufferData = BuildRendererPathFrameBufferData(camera, nullptr);
		UpdateConstantBufferRenderPath(cmd_list, rendererPath, rendererPathBufferData);
		EASY_END_BLOCK

		auto rt_output = rendererPath->GetColorRenderTarget().get();
		if (camera)
		{
			EASY_BLOCK("Pass_MaterialPass")
			Pass_SelectedAssetViewResourcePass(cmd_list, rendererPath);
			EASY_END_BLOCK
		}
		else
		{
			// if there is no camera, clear to black and and render the performance metrics
			GetCmdList()->ClearRenderTarget(rt_output, 0, 0, false, Color::standard_black);
		}

		// transition the render target to a readable state so it can be rendered
		// within the viewport or copied to the swap chain back buffer
		rt_output->SetLayout(RHI_Image_Layout::Shader_Read, cmd_list);
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
			EASY_BLOCK("Pass_SkyBox")
			Pass_SkyBox(cmd_list, rendererPath);
			EASY_END_BLOCK

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
		rt_output->SetLayout(RHI_Image_Layout::Shader_Read, cmd_list);
	}

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

		if (width > m_resolution_output.x || height > m_resolution_output.y)
		{
			DEBUG_LOG_WARN("Can't set {}x{} as it's larger then the output resolution {}x{}",
				width, height, static_cast<uint32_t>(m_resolution_output.x), static_cast<uint32_t>(m_resolution_output.y));
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
			CreateRenderTargets(true, false, true);

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
			CreateRenderTargets(false, true, true);

			// Re-create samplers
			CreateSamplers(true);
		}

		// Log
		DEBUG_LOG_INFO("Output resolution output has been set to %dx%d", width, height);
	}

	void Renderer::PushPassConstants(RHI_CommandList* cmd_list)
	{
		cmd_list->PushConstants(0, sizeof(Pcb_Pass), &m_cb_pass_cpu);
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

			m_cb_light_arr_cpu.lightArr[index].intensity = light->GetIntensityWatt(rendererPath->GetRenderCamera());
			m_cb_light_arr_cpu.lightArr[index].color = light->GetColor();
			m_cb_light_arr_cpu.lightArr[index].range = light->GetRange();
			m_cb_light_arr_cpu.lightArr[index].angle = light->GetAngle();
			m_cb_light_arr_cpu.lightArr[index].bias = light->GetBias();
			m_cb_light_arr_cpu.lightArr[index].normal_bias = light->GetNormalBias();

			m_cb_light_arr_cpu.lightArr[index].position = light->GetGameObject()->GetComponent<Transform>()->GetPosition();
			m_cb_light_arr_cpu.lightArr[index].direction = light->GetGameObject()->GetComponent<Transform>()->GetForward();

			m_cb_light_arr_cpu.lightArr[index].flags = 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= light->GetLightType() == LightType::Directional ? (1 << 0) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= light->GetLightType() == LightType::Point ? (1 << 1) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= light->GetLightType() == LightType::Spot ? (1 << 2) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= light->GetShadowsEnabled() ? (1 << 3) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= light->GetShadowsTransparentEnabled() ? (1 << 4) : 0;
			/*m_cb_light_arr_cpu.lightArr[index].flags |= light->GetShadowsScreenSpaceEnabled() ? (1 << 5) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= light->GetVolumetricEnabled() ? (1 << 5) : 0;*/
		}

		GetConstantBuffer(Renderer_ConstantBuffer::LightArr)->Update(&m_cb_light_arr_cpu);
	}

	void Renderer::UpdateDefaultConstantBufferLightArr(RHI_CommandList* cmd_list, const int lightCount, RendererPath* rendererPath)
	{
		m_cb_light_arr_cpu.lightCount = lightCount;

		for (int index = 0; index < lightCount; index++)
		{
			// todo only one light has shadow, is temp code
			for (uint32_t i = 0; i < rendererPath->GetShadowArraySize(); i++)
			{
				m_cb_light_arr_cpu.lightArr[index].view_projection[i] = rendererPath->GetLightViewMatrix(i) * rendererPath->GetLightProjectionMatrix(i);
			}

			m_cb_light_arr_cpu.lightArr[index].intensity = 2500.0f;
			m_cb_light_arr_cpu.lightArr[index].color = Color::White;
			m_cb_light_arr_cpu.lightArr[index].range = 200.0f;
			m_cb_light_arr_cpu.lightArr[index].angle = 0.5;
			m_cb_light_arr_cpu.lightArr[index].bias = 0.0005;

			m_cb_light_arr_cpu.lightArr[index].normal_bias = 5.0;
			m_cb_light_arr_cpu.lightArr[index].position = Vector3::Zero;
			m_cb_light_arr_cpu.lightArr[index].direction = Quaternion::FromAngleAxis(30.0f, Vector3::Forward) * Vector3::Forward;
			m_cb_light_arr_cpu.lightArr[index].flags = 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= (1 << 0);
			m_cb_light_arr_cpu.lightArr[index].flags |= false ? (1 << 3) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= false ? (1 << 4) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= false ? (1 << 5) : 0;
		}

		GetConstantBuffer(Renderer_ConstantBuffer::LightArr)->Update(&m_cb_light_arr_cpu);
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

	void Renderer::OnSyncPoint(RHI_CommandList* cmd_list)
	{	
		// is_sync_point: the command pool has exhausted its command lists and 
		// is about to reset them, this is an opportune moment for us to perform
		// certain operations, knowing that no rendering commands are currently
		// executing and no resources are being used by any command list
		m_resource_index++;
		bool is_sync_point = m_resource_index == resources_frame_lifetime;
		if (is_sync_point)
		{
			m_resource_index = 0;

			for (shared_ptr<RHI_ConstantBuffer> constant_buffer : GetConstantBuffers())
			{
				constant_buffer->ResetOffset();
			}

			// reset dynamic buffer offsets
			GetStructuredBuffer(Renderer_StructuredBuffer::Spd)->ResetOffset();

			// delete any rhi resources that have accumulated
			if (RHI_Device::DeletionQueueNeedsToParse())
			{
				RHI_Device::QueueWaitAll();
				RHI_Device::DeletionQueueParse();
				DEBUG_LOG_INFO("Parsed deletion queue");
			}
			{
			
			// reset dynamic buffer offsets
	/*		GetStructuredBuffer(Renderer_StructuredBuffer::Spd)->ResetOffset();
			GetConstantBufferFrame()->ResetOffset();*/

			/*if (bindless_materials_dirty)
			{
				RHI_Device::UpdateBindlessResources(nullptr, &bindless_textures);
				bindless_materials_dirty = false;
			}*/	
			}
		}

		// update frame constant buffer // todo
		EASY_BLOCK("Build cb_frame")
		Cb_Frame frameBufferData = BuildFrameBufferData();
		GetConstantBuffer(Renderer_ConstantBuffer::Frame)->Update(&frameBufferData);
		EASY_END_BLOCK

		// generate mips - if any
		{
			lock_guard lock(mutex_mip_generation);
			for (RHI_Texture* texture : textures_mip_generation)
			{
				Pass_GenerateMips(cmd_list, texture);
			}
			textures_mip_generation.clear();
		}

		// filter environment on directional light change
		{
		/*	static Quaternion rotation;
			static float intensity;
			static Color color;

			for (const shared_ptr<Entity>& entity : m_renderables[Renderer_Entity::Light])
			{
				if (const shared_ptr<Light>& light = entity->GetComponent<Light>())
				{
					if (light->GetLightType() == LightType::Directional)
					{
						if (light->GetEntity()->GetRotation() != rotation ||
							light->GetIntensityLumens() != intensity ||
							light->GetColor() != color
							)
						{
							rotation = light->GetEntity()->GetRotation();
							intensity = light->GetIntensityLumens();
							color = light->GetColor();

							m_environment_mips_to_filter_count = GetRenderTarget(Renderer_RenderTarget::skysphere)->GetMipCount() - 1;
						}
					}
				}
			}*/
		}
	}

	void Renderer::SetOption(Renderer_Option option, float value)
	{
		  // clamp value
        {
            // anisotropy
            if (option == Renderer_Option::Anisotropy)
            {
                value = Helper::Clamp(value, 0.0f, 16.0f);
            }
            // shadow resolution
            else if (option == Renderer_Option::ShadowResolution)
            {
                value = Helper::Clamp(value, static_cast<float>(resolution_shadow_min), static_cast<float>(RHI_Device::PropertyGetMaxTexture2dDimension()));
            }
            else if (option == Renderer_Option::ResolutionScale)
            {
                value = Helper::Clamp(value, 0.5f, 1.0f);
            }
        }

        // early exit if the value is already set
        if ((m_options.find(option) != m_options.end()) && m_options[option] == value)
            return;

        // reject changes (if needed)
        {
           /* if (option == Renderer_Option::Hdr)
            {
                if (value == 1.0f)
                {
                    if (!Display::GetHdr())
                    { 
                        DEBUG_INFO("This display doesn't support HDR");
                        return;
                    }
                }
            }
            else if (option == Renderer_Option::VariableRateShading)
            {
                if (value == 1.0f)
                {
                    if (!RHI_Device::PropertyIsShadingRateSupported())
                    { 
                        SP_LOG_INFO("This GPU doesn't support variable rate shading");
                        return;
                    }
                }
            }*/
        }

        // set new value
        m_options[option] = value;

        // handle cascading changes
        {
            // antialiasing
            if (option == Renderer_Option::Antialiasing)
            {
                bool taa_enabled = value == static_cast<float>(Renderer_Antialiasing::Taa) || value == static_cast<float>(Renderer_Antialiasing::TaaFxaa);
                bool fsr_enabled = GetOption<Renderer_Upsampling>(Renderer_Option::Upsampling) == Renderer_Upsampling::Fsr2;

                if (taa_enabled)
                {
					// todo
                    //// implicitly enable FSR since it's doing TAA.
                    //if (!fsr_enabled)
                    //{
                    //    m_options[Renderer_Option::Upsampling] = static_cast<float>(Renderer_Upsampling::Fsr2);
                    //    RHI_FidelityFX::FSR2_ResetHistory();
                    //}
                }
                else
                {
                    // implicitly disable FSR since it's doing TAA
                    if (fsr_enabled)
                    {
                        m_options[Renderer_Option::Upsampling] = static_cast<float>(Renderer_Upsampling::Linear);
                    }
                }
            }
            // upsampling
            else if (option == Renderer_Option::Upsampling)
            {
                bool taa_enabled = GetOption<Renderer_Antialiasing>(Renderer_Option::Antialiasing) == Renderer_Antialiasing::Taa;

                if (value == static_cast<float>(Renderer_Upsampling::Linear))
                {
                    // Implicitly disable TAA since FSR 2.0 is doing it
                    if (taa_enabled)
                    {
                        m_options[Renderer_Option::Antialiasing] = static_cast<float>(Renderer_Antialiasing::Disabled);
                        DEBUG_LOG_INFO("Disabled TAA since it's done by FSR 2.0.");
                    }
                }
      //          else if (value == static_cast<float>(Renderer_Upsampling::Fsr2))
      //          {
      //              // Implicitly enable TAA since FSR 2.0 is doing it
      //              if (!taa_enabled)
      //              {
      //                  m_options[Renderer_Option::Antialiasing] = static_cast<float>(Renderer_Antialiasing::Taa);
      //                  RHI_FidelityFX::FSR2_ResetHistory();
						//DEBUG_LOG_INFO("Enabled TAA since FSR 2.0 does it.");
      //              }
      //          }
            }

        	// shadow resolution
            else if (option == Renderer_Option::ShadowResolution)
            {
				// todo:
               /* const auto& light_entities = m_renderables[Renderer_Entity::Light];
                for (const auto& light_entity : light_entities)
                {
                    auto light = light_entity->GetComponent<Light>();
                    if (light->IsFlagSet(LightFlags::Shadows))
                    {
                        light->RefreshShadowMap();
                    }
                }*/
            }
            else if (option == Renderer_Option::Hdr)
            {
                if (swap_chain)
                { 
                    swap_chain->SetHdr(value == 1.0f);
                }
            }
            else if (option == Renderer_Option::Vsync)
            {
                if (swap_chain)
                {
                    swap_chain->SetVsync(value == 1.0f);
                }
            }
            else if (option == Renderer_Option::FogVolumetric || option == Renderer_Option::ScreenSpaceShadows)
            {
				// todo
                // SP_FIRE_EVENT(EventType::LightOnChanged);
            }
            else if (option == Renderer_Option::PerformanceMetrics)
            {
               /* static bool enabled = false;
                if (!enabled && value == 1.0f)
                {
                    Profiler::ClearMetrics();
                }

                enabled = value != 0.0f;*/
            }
        }
	}

	unordered_map<Renderer_Option, float>& Renderer::GetOptions()
	{
		return m_options;
	}

	void Renderer::SetOptions(const unordered_map<Renderer_Option, float>& options)
	{
		m_options = options;
	}


	RHI_SwapChain* Renderer::GetSwapChain()
	{
		return swap_chain.get();
	}

	void Renderer::BlitToBackBuffer(RHI_CommandList* cmd_list, RHI_Texture* texture)
	{
		cmd_list->BeginMarker("blit_to_back_buffer");
		cmd_list->Blit(texture, swap_chain.get());
		cmd_list->EndMarker();
	}

	void Renderer::AddTextureForMipGeneration(RHI_Texture* texture)
	{
		lock_guard<mutex> guard(mutex_mip_generation);
		textures_mip_generation.push_back(texture);
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
		return GetRenderTarget(Renderer_RenderTarget::frame_output).get();
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
		// cb_frame_cpu.time = static_cast<float>(Timer::GetTimeSec());
		cb_frame_cpu.delta_time = static_cast<float>(Time::GetDeltaTime());
		//cb_frame_cpu.gamma = GetOption<float>(Renderer_Option:);
		cb_frame_cpu.frame = static_cast<uint32_t>(LitchiRuntime::frame_num);

		// These must match what Common_Buffer.hlsl is reading
		cb_frame_cpu.set_bit(GetOption<bool>(Renderer_Option::ScreenSpaceReflections), 1 << 0);
		//cb_frame_cpu.set_bit(GetOption<bool>(Renderer_Option::Ssgi), 1 << 1);
		//cb_frame_cpu.set_bit(GetOption<bool>(Renderer_Option::VolumetricFog), 1 << 2);
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
}
