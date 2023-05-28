
#ifndef UNTITLED_APPLICATION_BASE_H
#define UNTITLED_APPLICATION_BASE_H

#include <direct.h>
#include <string>

#include "Runtime/Function/Renderer/Core/Renderer.h"
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"

namespace LitchiRuntime
{

    class ApplicationBase {
    public:
        ApplicationBase() {}
        virtual ~ApplicationBase() {}

        void set_title(std::string title) { title_ = title; }

        const std::string& data_path() { return data_path_; }
        void SetDataPath(std::string data_path)
        {
            data_path_ = data_path;
            // data_path_= "D:/WorkSpace/LitchiEngineGit/LitchiEngineGithub/LitchiEngine/build/Engine/Source/Assets/";
        }

        /// 初始化OpenGL
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


        std::unique_ptr<Renderer>					renderer;

        std::unique_ptr<UniformBuffer>	engineUBO;
        std::unique_ptr<ModelManager> modelManager;
        std::unique_ptr<MaterialManager> materialManager;
        std::unique_ptr<TextureManager> textureManager;
        std::unique_ptr<ShaderManager> shaderManager;


        static ApplicationBase* Instance() { return instance_; }
        static ApplicationBase* instance_;
    protected:
        std::string title_;//标题栏显示

        std::string data_path_;//资源目录
    };
}

#endif //UNTITLED_APPLICATION_BASE_H
