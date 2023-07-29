
#include "ApplicationBase.h"

#include <memory>
#include <iostream>

#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Input/input.h"

#include "Runtime/Core/Meta/Reflection/type.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Physics/physics.h"

#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/Renderer/RenderSystem.h"

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

        UpdateScreenSize();

        RenderSystem::Initialize(new RenderSystem(),true,true);

        TypeManager::Initialize(new TypeManager());

        SerializerManager::Initialize(new SerializerManager());

        //初始化 fmod
        //Audio::Init();

        //初始化物理引擎
        Physics::Init();

        // 初始化场景 如果没有场景则构建默认场景
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

        this->sceneManager->Foreach([](GameObject* game_object) {
            if (game_object->GetActive()) {
                game_object->ForeachComponent([](Component* component) {
                    component->Update();
                    });
            }
            });

        Input::Update();
        //Audio::Update();
    }


    void ApplicationBase::Render() {

        RenderSystem::Instance()->Render();
        
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