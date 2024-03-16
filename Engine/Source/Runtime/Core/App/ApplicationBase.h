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

        /// 初始化
        virtual void Init();

        /// 初始化图形库，例如glfw
        virtual void InitGraphicsLibraryFramework();

        ///
        virtual void InitLuaBinding();

        virtual void LoadConfig();

        virtual void Run();

        /// 一帧
        virtual void OneFrame();

        virtual void UpdateScreenSize();

        /// 每一帧内逻辑代码。
        virtual void Update();

        virtual void FixedUpdate();

        /// 逻辑代码执行后，应用到渲染。
        virtual void Render();

        virtual void Exit();


        // std::unique_ptr<Renderer>					renderer;

        //std::unique_ptr<UniformBuffer>	engineUBO;
        
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

        std::string m_engineRoot{};// engine executable file root 
        std::string m_projectPath{};
        std::string m_projectName{};
        std::string m_title;// default: ProjectName.EngineVersion

    };
}
