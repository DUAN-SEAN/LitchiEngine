
#pragma once

#include "Renderer_Definitions.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Scene/SceneManager.h"


namespace LitchiRuntime
{

	enum RendererPathType
	{
		RendererPathType_Invalid,
		RendererPathType_SceneView,
		RendererPathType_GameView,
		RendererPathType_AssetView,
		RendererPathType_Custom,
		RendererPathType_Count
	};

	// rendererPath, completed once pass, render to texture
	class RendererPath
	{
	public:
		RendererPath(RendererPathType rendererPathType);
		~RendererPath();
	public:
		void UpdateRenderTarget(float width, float height);
		void UpdateRenderableGameObject();
		void UpdateLightShadow();

		RendererPathType GetRendererPathType(){	return m_rendererPathType;}

		RenderCamera* GetRenderCamera(){return m_renderCamera;}
		void UpdateRenderCamera(RenderCamera* camera);

		std::shared_ptr<RHI_Texture> GetColorRenderTarget() { return m_colorRenderTarget; }
		std::shared_ptr<RHI_Texture> GetDepthRenderTarget() { return m_depthRenderTarget; }

		void UpdateScene(Scene* scene);
		Scene* GetRenderScene() { return m_renderScene; }

		const std::unordered_map<Renderer_Entity, std::vector<GameObject*>>& GetRenderables() { return m_renderables; }

		bool GetActive()const { return m_active; }
		void SetActive(bool isActive) {  m_active = isActive; }

		UICanvas* GetCanvas();
	public:

		const Matrix& GetLightViewMatrix(uint32_t index = 0) const;
		const Matrix& GetLightProjectionMatrix(uint32_t index = 0) const;

		RHI_Texture* GetShadowDepthTexture() const { return m_shadow_map.texture_depth.get(); }
		RHI_Texture* GetShadowColorTexture() const { return m_shadow_map.texture_color.get(); }
		uint32_t GetShadowArraySize() const;
		bool IsInLightViewFrustum(MeshFilter* renderable, uint32_t index) const;

	private:

		void CreateColorRenderTarget();
		void CreateDepthRenderTarget();
		std::string GetRenderPathName();
		bool CheckIsBuildInRendererCamera();

		bool CheckShadowMapNeedRecreate();
		void CreateShadowMap();
		void ComputeCascadeSplits(RenderCamera* renderCamera);
		void ComputeLightViewMatrix();
		void ComputeLightProjectionMatrix(uint32_t index = 0);

		float m_width;
		float m_height;

		RendererPathType m_rendererPathType = RendererPathType_Invalid;
		RenderCamera* m_renderCamera = nullptr;
		Scene* m_renderScene = nullptr;

		// UI provide Camera
		RenderCamera* m_renderCamera4UI;

		// one camera to light

		// Light
		Light* m_mainLight = nullptr;
		std::array<Matrix, 6> m_matrix_view;
		std::array<Matrix, 6> m_matrix_projection;

		// shadow
		uint32_t m_cascade_count = 4;
		ShadowMap m_shadow_map; // correct is <tuple<light,camera>,shadowMap>
		bool m_last_shadows_enabled = false;
		bool m_last_shadows_transparent_enabled = false;

		std::shared_ptr<RHI_Texture> m_depthRenderTarget = nullptr;
		std::shared_ptr<RHI_Texture> m_colorRenderTarget = nullptr;

		std::unordered_map<Renderer_Entity, std::vector<GameObject*>> m_renderables;

		bool m_active {false};
	};

}
