#pragma once

#include <direct.h>
#include <string>
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Core/Window/Window.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Resource/FontManager.h"
#include "Runtime/Resource/PrefabManager.h"
#include "Runtime/Resource/TextureManager.h"
#include "Runtime/Resource/ConfigManager.h"

namespace LitchiRuntime
{
	class Window;

	/**
	 * @brief Litchi Engine Application Type
	*/
	enum class LitchiApplicationType
	{
		Editor,
		Game
	};

	/**
	 * @brief Application Instance Base
	*/
	class ApplicationBase {
	public:

		/**
		 * @brief ctor
		*/
		ApplicationBase() {}

		/**
		 * @brief cctor
		 * call when application exit
		*/
		virtual ~ApplicationBase() {}

		/**
		 * @brief Set Application Title
		 * @param title
		*/
		void SetTitle(std::string title) { m_title = title; }

		/**
		 * @brief Get Project Path
		 * @return Project Path
		*/
		const std::string& GetProjectPath() { return m_projectPath; }

		/**
		 * @brief Set Project Path
		 * @param projectPath
		*/
		void SetProjectPath(const std::string& projectPath)
		{
			m_projectPath = projectPath;
		}

		/**
		 * @brief Get Engine Assets Path
		 * @return
		*/
		const std::string& GetEngineAssetsPath()
		{
			return m_engineAssetsPath;
		}

		/**
		 * @brief Get Engine Execute Path
		 * @return
		*/
		const std::string& GetEngineRootPath()
		{
			return m_engineRootPath;
		}

		/**
		 * @brief Get Litchi Engine Application Type
		 * @return
		*/
		virtual LitchiApplicationType GetApplicationType() = 0;

		/**
		 * @brief When Engine Run, Initialize Application Instance
		 * @return return true if Initialize done
		*/
		virtual bool Initialize();

		/**
		 * @brief Run Application, Call Before Initialize
		*/
		virtual void Run();

		/**
		 * @brief Per Frame Call, Call by Run。
		*/
		virtual void Update();

		/**
		 * @brief Exit Application
		*/
		virtual void Exit();

		/**
		 * @brief Create Window Setting, Must override
		 * @return window setting
		*/
		virtual WindowSettings CreateWindowSettings() = 0;

		/**
		 * @brief Called callback function , When SceneManager Load Scene
		*/
		virtual void OnSceneLoaded();
		virtual void OnApplyProjectSettings();
		virtual void OnResetProjectSettings();

		/**
		 * @brief Config Manager, Include Project Config
		*/
		std::unique_ptr<ConfigManager> configManager;

		/**
		 * @brief window handle
		*/
		std::unique_ptr<Window> window;

		/* Resource Managers */
		std::unique_ptr<ModelManager> modelManager;
		std::unique_ptr<ShaderManager> shaderManager;
		std::unique_ptr<MaterialManager> materialManager;
		std::unique_ptr<FontManager> fontManager;
		std::unique_ptr<TextureManager> textureManager;
		std::unique_ptr<SceneManager> sceneManager;
		std::unique_ptr<PrefabManager> prefabManager;

		/**
		 * @brief Return Current Application Singleton Instance
		 * @return
		*/
		static ApplicationBase* Instance() { return s_instance; }

		/**
		 * @brief  Application Singleton Instance
		*/
		static ApplicationBase* s_instance;

	protected:

		/**
		 * @brief Engine Assets Path
		 * Location in the engine's execution directory
		*/
		std::string m_engineAssetsPath;

		/**
		 * @brief engine's execution directory
		*/
		std::string m_engineRootPath{};

		/**
		 * @brief Project Root Path
		*/
		std::string m_projectPath{};

		/**
		 * @brief Project Name
		*/
		std::string m_projectName{};

		/**
		 * @brief ProjectName.EngineVersion
		*/
		std::string m_title;

	};
}
