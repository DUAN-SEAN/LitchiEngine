
#pragma once

#include <memory>

#include "Runtime/Function/UI/UIManager/UIManager.h"

#include "Core/EditorActions.h"
#include "Core/PanelsManager.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/Rendering/RendererPath.h"

using namespace LitchiRuntime;

namespace LitchiEditor
{
	class ApplicationEditor :public ApplicationBase
	{
	public:
		ApplicationEditor();
		~ApplicationEditor();
		ApplicationType GetApplicationType() override;
		bool Init() override;
		void Run() override;
		void Update() override;

		WindowSettings CreateWindowSettings() override;

		/**
		* Returns true if the app is running
		*/
		bool IsRunning() const;
		static ApplicationEditor* Instance() { return instance_; }


		const std::string& GetEditorAssetsPath()
		{
			return m_editorAssetsPath;
		}


	public:
		// 提供一系列的绘制函数
		void RenderViews(float p_deltaTime);

		// 如 RenderUI
		void RenderUI();

	public:

		std::unique_ptr<UIManager>		uiManager;
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

	protected:
		std::string m_editorAssetsPath;

	private:
		void RunProjectHub();
		void OnProjectOpen();
		void SetupEditorUI();
		void SetupRendererPath();

	private:

		float m_restFixedTime = 0.0f;

		uint64_t m_elapsedFrames = 0;
		Canvas			m_canvas;
		static ApplicationEditor* instance_;

		EditorActions	m_editorActions;
	};
}
