
#pragma once

#include <memory>

#include "Runtime/Core/Window/Window.h"
#include "Runtime/Core/Window/Context/Device.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/UI/UIManager/UIManager.h"

#include "Core/EditorActions.h"
#include "Core/EditorRenderer.h"
#include "Core/EditorResources.h"
#include "Core/PanelsManager.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/Rendering/RendererPath.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"

using namespace LitchiRuntime;

namespace LitchiEditor
{
	class ApplicationEditor :public ApplicationBase
	{
	public:
		ApplicationEditor();
		~ApplicationEditor();
		void Init() override;
		void Run() override;
		void Update() override;
		/**
		* Returns true if the app is running
		*/
		bool IsRunning() const;
		static ApplicationEditor* Instance() { return instance_; }


	public:
		// 提供一系列的绘制函数
		void RenderViews(float p_deltaTime);

		// 如 RenderUI
		void RenderUI();

	public:

		void SetSelectGameObject(GameObject* selectedGO) { m_selectGO = selectedGO; }
		GameObject* GetSelectGameObject() { return m_selectGO; }
		
		std::unique_ptr<UIManager>		uiManager;
		// std::unique_ptr<EditorResources>		editorResources;

		//std::unique_ptr<ShaderStorageBuffer>	lightSSBO;
		//std::unique_ptr<ShaderStorageBuffer>	simulatedLightSSBO;

		/*Resource::Shader* m_shadowMapShader;
		Resource::Shader* m_shadowMapShader4Skinned;*/
		//std::unique_ptr<EditorRenderer> editorRenderer;

		PanelsManager	m_panelsManager;
		
		RendererPath* m_rendererPath4SceneView = nullptr;
		RendererPath* m_rendererPath4GameView = nullptr;
		RendererPath* m_rendererPath4AssetView = nullptr;

	public:
		/**
		* Select an actor and show him in inspector
		* @param p_target
		*/
		void SelectActor(GameObject* p_target);
		void MoveToTarget(GameObject* p_target);
	private:
		void SetupUI();
		void SetupRendererPath();

	private:

		float m_restFixedTime = 0.0f;

		uint64_t m_elapsedFrames = 0;
		Canvas			m_canvas;
		static ApplicationEditor* instance_;

		GameObject* m_selectGO = nullptr;
		EditorActions	m_editorActions;
	};
}
