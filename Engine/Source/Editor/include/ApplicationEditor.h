
#pragma once

#include <memory>

#include "Runtime/Core/App/application_base.h"
#include "Runtime/Core/Window/Window.h"
#include "Runtime/Core/Window/Context/Device.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/UI/UIManager/UIManager.h"
#include <Runtime/Function/Scene/scene_manager.h>

#include "Core/PanelsManager.h"
#include "Runtime/Function/Renderer/Driver.h"

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

		std::unique_ptr<Device>				device;
		std::unique_ptr<Driver>				driver;
		std::unique_ptr<Window>				window;
		std::unique_ptr<InputManager>		inputManager;
		std::unique_ptr<UIManager>		uiManager;

		SceneManager* sceneManager;

		const std::string projectPath;
		const std::string projectName;
		const std::string projectFilePath;
		const std::string engineAssetsPath;
		const std::string projectAssetsPath;
		const std::string projectScriptsPath;
		const std::string editorAssetsPath;

	private:
		void SetupUI();

	private:

		uint64_t m_elapsedFrames = 0;
		Canvas			m_canvas;
		PanelsManager	m_panelsManager;
		static ApplicationEditor* instance_;

		GameObject* m_selectGO = nullptr;
	};
}
