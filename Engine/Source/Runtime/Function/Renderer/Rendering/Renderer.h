
#pragma once

//= INCLUDES ========================
#include "Renderer_Definitions.h"
#include "../RHI/RHI_Texture.h"
#include "Runtime/Core/Math/Rectangle.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Vector4.h"
#include "Runtime/Core/Math/Plane.h"
#include <unordered_map>
//#include "Event.h"
#include "Mesh.h"
#include "Renderer_ConstantBuffers.h"
#include "Font/Font.h"
#include "Grid.h"
#include "PlaneGeometry.h"
#include "RendererPath.h"
#include "SphereGeometry.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
//===================================

namespace LitchiRuntime
{
	//= FWD DECLARATIONS =
	class Window;
	class GameObject;
	class RenderCamera;
	class Light;
	class Environment;
	//namespace Math
	//{
	class BoundingBox;
	class Frustum;
	//}
	//====================


	class LC_CLASS Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();
		static void Tick();

		// Primitive rendering (excellent for debugging)
		static void DrawLine(const Vector3& from, const Vector3& to, const Vector4& color_from = DEBUG_COLOR, const Vector4& color_to = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);
		static void DrawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);
		static void DrawRectangle(RenderCamera* camera,const Rectangle& rectangle, const Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);
		static void DrawBox(const BoundingBox& box, const Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);
		static void DrawCircle(const Vector3& center, const Vector3& axis, const float radius, uint32_t segment_count, const Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);
		static void DrawSphere(const Vector3& center, float radius, uint32_t segment_count, const Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);
		static void DrawDirectionalArrow(const Vector3& start, const Vector3& end, float arrow_size, const Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);
		static void DrawPlane(const Plane& plane, const Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depth = true);

		// Options
		template<typename T>
		static T GetOption(const Renderer_Option option) { return static_cast<T>(GetOptions()[option]); }
		static void SetOption(Renderer_Option option, float value);
		static std::unordered_map<Renderer_Option, float>& GetOptions();
		static void SetOptions(const std::unordered_map<Renderer_Option, float>& options);

		// Swapchain
		static RHI_SwapChain* GetSwapChain();
		static void BlitToBackBuffer(RHI_CommandList* cmd_list, RHI_Texture* texture);

		// Mip generation
		static void AddTextureForMipGeneration(RHI_Texture* texture);
		static void Pass_GenerateMips(RHI_CommandList* cmd_list, RHI_Texture* texture);

		// Misc
		static void SetStandardResources(RHI_CommandList* cmd_list);
		static uint64_t GetFrameNum();
		static RHI_Api_Type GetRhiApiType();

		////= RESOLUTION/SIZE =============================================================================
		//// Viewport
		//static const RHI_Viewport& GetViewport();
		//static void SetViewport(float width, float height);

		// Resolution render
		static const Vector2& GetResolutionRender();
		static void SetResolutionRender(uint32_t width, uint32_t height, bool recreate_resources = true);

		// Resolution output
		static const Vector2& GetResolutionOutput();
		static void SetResolutionOutput(uint32_t width, uint32_t height, bool recreate_resources = true);
		//===============================================================================================

		//= RHI RESOURCES====================
		static RHI_CommandList* GetCmdList();
		//===================================

		//= RESOURCES ===========================================================================================
		static RHI_Texture* GetFrameTexture();

		// Get all
		static std::array<std::shared_ptr<RHI_Texture>, static_cast<uint32_t>(Renderer_RenderTarget::max)>& GetRenderTargets();
		static std::array<std::shared_ptr<RHI_Shader>, static_cast<uint32_t>(Renderer_Shader::max)>& GetShaders();
		static std::array<std::shared_ptr<RHI_ConstantBuffer>, static_cast<uint32_t>(Renderer_ConstantBuffer::Max)>& GetConstantBuffers();
		static std::array<std::shared_ptr<RHI_StructuredBuffer>, 2>& GetStructuredBuffers();

		// Get individual
		static std::shared_ptr<RHI_RasterizerState> GetRasterizerState(const Renderer_RasterizerState type);
		static std::shared_ptr<RHI_DepthStencilState> GetDepthStencilState(const Renderer_DepthStencilState type);
		static std::shared_ptr<RHI_BlendState> GetBlendState(const Renderer_BlendState type);
		static std::shared_ptr<RHI_Texture> GetRenderTarget(const Renderer_RenderTarget type);
		static std::shared_ptr<RHI_Shader> GetShader(const Renderer_Shader type);
		static std::shared_ptr<RHI_Sampler> GetSampler(const Renderer_Sampler type);
		static std::shared_ptr<RHI_ConstantBuffer> GetConstantBuffer(const Renderer_ConstantBuffer type);
		static std::shared_ptr<RHI_StructuredBuffer> GetStructuredBuffer(const Renderer_StructuredBuffer type);
		static std::shared_ptr<RHI_Texture> GetStandardTexture(const Renderer_StandardTexture type);
		static std::shared_ptr<Mesh> GetStandardMesh(const Renderer_MeshType type);
		//=======================================================================================================

		// RendererPath
		static void UpdateRendererPath(RendererPathType rendererPathType, RendererPath* rendererPath);
		//static RenderCamera* GetMainCamera();

	private:
		// Constant buffers
		static void PushPassConstants(RHI_CommandList* cmd_list);
		//static void UpdateConstantBufferLightArr(RHI_CommandList* cmd_list, Light** lightArr,const int lightCount, RendererPath* rendererPath);// todo remove
		//static void UpdateDefaultConstantBufferLightArr(RHI_CommandList* cmd_list,const int lightCount, RendererPath* rendererPath);// todo remove
		static void SetMaterialBuffer(RHI_CommandList* cmd_list, Material* material);
		static void UpdateConstantBufferRenderPath(RHI_CommandList* cmd_list, RendererPath* rendererPath, Cb_RendererPath& renderPathBufferData);

		// Resource creation
		static void CreateConstantBuffers();
		static void CreateStructuredBuffers();
		static void CreateDepthStencilStates();
		static void CreateRasterizerStates();
		static void CreateBlendStates();
		static void CreateFonts();
		static void CreateStandardMeshes();
		static void CreateStandardTextures();
		static void CreateSkyBoxMesh();
		static void CreateShaders();
		static void CreateSamplers(const bool create_only_anisotropic = false);
		static void CreateRenderTargets(const bool create_render, const bool create_output, const bool create_dynamic);
		static void LoadDefaultMaterials();

		// Passes - Core
		static void Render4BuildInSceneView(RHI_CommandList* cmd_list, RendererPath* rendererPath);
		static void Render4BuildInAssetView(RHI_CommandList* cmd_list, RendererPath* rendererPath);
		static void Render4BuildInGameView(RHI_CommandList* cmd_list, RendererPath* rendererPath);
		static void Pass_ShadowMaps(RHI_CommandList* cmd_list, RendererPath* rendererPath,const bool is_transparent_pass);
		static void Pass_SkyBox(RHI_CommandList* cmd_list, RendererPath* rendererPath);
		static void Pass_ForwardPass(RHI_CommandList* cmd_list, RendererPath* rendererPath, const bool is_transparent_pass);
		static void Pass_UIPass(RHI_CommandList* cmd_list, RendererPath* rendererPath);
		// passes - debug/editor
		static void Pass_GridPass(RHI_CommandList* cmd_list, RendererPath* rendererPath);
		static void Pass_IconPass(RHI_CommandList* cmd_list, RendererPath* rendererPath, Cb_RendererPath& rendererPathBufferData);
		static void Pass_SelectedAssetViewResourcePass(RHI_CommandList* cmd_list, RendererPath* rendererPath);

		// Buffer
		static Cb_Frame BuildFrameBufferData();
		static Cb_RendererPath BuildRendererPathFrameBufferData(RenderCamera* camera, UICanvas* canvas);

		// Event handlers
		// static void OnFullScreenToggled();
		static void OnSyncPoint(RHI_CommandList* cmd_list);

		static void DestroyResources();

		static RHI_CommandPool* m_cmd_pool;
		static uint32_t m_resource_index;

		// RendererPath container
		static std::unordered_map<RendererPathType, RendererPath*> m_rendererPaths;

		static Pcb_Pass m_cb_pass_cpu;
		static Sb_Light m_cb_light_cpu;
		static  Cb_Light_Arr m_cb_light_arr_cpu;
		static std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer_skyBox;
		static std::shared_ptr<RHI_IndexBuffer> m_index_buffer_skyBox;

		static bool m_brdf_specular_lut_rendered;

		static std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer_lines;
		static std::vector<RHI_Vertex_PosCol> m_line_vertices;
		static std::vector<float> m_lines_duration;
		static uint32_t m_lines_index_depth_off;
		static uint32_t m_lines_index_depth_on;

		static Material* m_default_standard_material;
		static Material* m_default_standard_skin_material;
	};
}
