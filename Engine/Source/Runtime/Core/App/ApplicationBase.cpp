
#include "ApplicationBase.h"

#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

#include "Runtime/Core/Meta/Reflection/type.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/Physics/physics.h"

#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"

//#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    ApplicationBase* ApplicationBase::s_instance;
    void ApplicationBase::Init() {
        s_instance = this;

        Debug::Init();

        DEBUG_LOG_INFO("game start");

        // 第二个参数支持后续修改
        ConfigManager::Initialize(new ConfigManager(), "ProjectConfig.Litchi");

        Time::Init();

        //初始化图形库，例如glfw
        InitGraphicsLibraryFramework();
        
        WindowSettings windowSettings;
        windowSettings.title = "Litchi Editor";
        windowSettings.width = 1280;
        windowSettings.height = 720;
        windowSettings.maximized = true;
        // 初始化Window
        window = std::make_unique<Window>(windowSettings);
        {
            auto iconPath = editorAssetsPath + "Icon.png";
            int iconWidth = 30;
            int iconHeight = 30;
            int iconChannel = 3;
            unsigned char* dataBuffer = stbi_load(iconPath.c_str(), &iconWidth, &iconHeight, &iconChannel, 4);
            window->SetIconFromMemory(reinterpret_cast<uint8_t*>(dataBuffer), iconWidth, iconHeight);
            window->MakeCurrentContext();
        }
        inputManager = std::make_unique<InputManager>(*window);

        UpdateScreenSize();

        {

            ResourceCache::Initialize();
            Renderer::Initialize();

        }

        TypeManager::Initialize(new TypeManager());
        SerializerManager::Initialize(new SerializerManager());

        //初始化 fmod
        //Audio::Init();

        //初始化物理引擎
        Physics::Init();

        ScriptEngine::Init(m_dataPath);

        // 初始化ResourceManager
        //modelManager = std::make_unique<ModelManager>();
        // materialManager = std::make_unique<MaterialManager>();
        //shaderManager = std::make_unique<ShaderManager>();

        //ServiceLocator::Provide<ModelManager>(*modelManager);
        // LitchiRuntime::ServiceLocator::Provide<MaterialManager>(*materialManager);
        // ServiceLocator::Provide<ShaderManager>(*shaderManager);

        // 初始化场景 如果没有场景则构建默认场景
        sceneManager = std::make_unique<SceneManager>();
    }

    /// 初始化图形库，例如glfw
    void ApplicationBase::InitGraphicsLibraryFramework() {

    }

    void ApplicationBase::InitLuaBinding() {
    }

    void ApplicationBase::LoadConfig() {
    }

    void ApplicationBase::Run() {

    }

    void ApplicationBase::Update() {
        Time::Update();
        UpdateScreenSize();

        auto scene = this->sceneManager->GetCurrentScene();
        if(scene)
        {
            scene->Tick();
        }
        
        // Input::Update();
        //Audio::Update();
    }


    void ApplicationBase::Render() {

        // RenderSystem::Instance()->Render();
        Renderer::Tick();
    }

    void ApplicationBase::FixedUpdate() {
        //Physics::FixedUpdate();
        

        this->sceneManager->Foreach([](GameObject* game_object) {
            if (game_object->GetActive()) {
                game_object->ForeachComponent([](Component* component) {
                    component->FixedUpdate();
                    });
            }
            });
    }

    void ApplicationBase::OneFrame() {
        Update();
        // 如果一帧卡了很久，就多执行几次FixedUpdate
        float cost_time = Time::delta_time();
        while (cost_time >= Time::fixed_update_time()) {
            FixedUpdate();
            cost_time -= Time::fixed_update_time();
        }

        Render();
    }

    void ApplicationBase::UpdateScreenSize() {
    }

    void ApplicationBase::Exit() {
    }
}