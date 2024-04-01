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

    enum class LitchiApplicationType
    {
        Editor,
        Game
    };

	class ApplicationBase {
    public:
        ApplicationBase(){}
        virtual ~ApplicationBase() {}

        void SetTitle(std::string title) { m_title = title; }

        const std::string& GetProjectPath() { return m_projectPath; }
        void SetProjectPath(std::string project_path)
        {
            m_projectPath = project_path;
        }

        const std::string& GetEngineAssetsPath()
        {
            return m_engineAssetsPath;
        }

        const std::string& GetEngineRootPath()
        {
            return m_engineRootPath;
        }


        virtual LitchiApplicationType GetApplicationType() = 0;

        // Init 
        virtual bool Initialize();

        // Call Before Init
        virtual void Run();

        // Per Frame Call, Call by Run。
        virtual void Update();

        // Exit
        virtual void Exit();

        // Create Window Setting, Must Impl
        virtual WindowSettings CreateWindowSettings() = 0;

		std::unique_ptr<ConfigManager> configManager;
		std::unique_ptr<ModelManager> modelManager;
        std::unique_ptr<ShaderManager> shaderManager;
        std::unique_ptr<MaterialManager> materialManager;
        std::unique_ptr<FontManager> fontManager;
        std::unique_ptr<TextureManager> textureManager;
        std::unique_ptr<Window> window;
        std::unique_ptr<SceneManager> sceneManager;
        std::unique_ptr<PrefabManager> prefabManager;

        static ApplicationBase* Instance() { return s_instance; }
        static ApplicationBase* s_instance;

    protected:

        std::string m_engineAssetsPath;

        std::string m_engineRootPath{};// engine executable file root 
        std::string m_projectPath{};
        std::string m_projectName{};
        std::string m_title;// default: ProjectName.EngineVersion

    };
}
