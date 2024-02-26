
#pragma once

#include "Renderer_Definitions.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Scene/SceneManager.h"


namespace LitchiRuntime
{

	enum RendererPathType
	{
		RendererPathType_Invalid,
		RendererPathType_SceneView,
		RendererPathType_GameView,
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

		RendererPathType GetRendererPathType(){	return m_rendererPathType;}

		RenderCamera* GetRenderCamera(){return m_renderCamera;}
		void SetRenderCamera(RenderCamera* camera){m_renderCamera = camera;}

		std::shared_ptr<RHI_Texture> GetColorRenderTarget() { return m_colorRenderTarget; }
		std::shared_ptr<RHI_Texture> GetDepthRenderTarget() { return m_depthRenderTarget; }

		void SetScene(Scene* scene) { m_renderScene = scene; }
		Scene* GetRenderScene() { return m_renderScene; }

		const std::unordered_map<Renderer_Entity, std::vector<GameObject*>>& GetRenderables() { return m_renderables; }

	private:

		void CreateColorRenderTarget();
		void CreateDepthRenderTarget();
		std::string GetRenderPathName();
		bool CheckIsBuildInRendererCamera();

		float m_width;
		float m_height;

		RendererPathType m_rendererPathType = RendererPathType_Invalid;
		RenderCamera* m_renderCamera = nullptr;
		Scene* m_renderScene = nullptr;

		// UI provide Camera
		RenderCamera* m_renderCamera4UI;

		std::shared_ptr<RHI_Texture> m_depthRenderTarget = nullptr;
		std::shared_ptr<RHI_Texture> m_colorRenderTarget = nullptr;


		std::unordered_map<Renderer_Entity, std::vector<GameObject*>> m_renderables;
	};

}
