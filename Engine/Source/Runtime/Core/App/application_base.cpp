﻿
#include <memory>
#include <iostream>
#include <glad/glad.h>

#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Input/input.h"
#include "application_base.h"

#include "Runtime/Core/Meta/Reflection/type.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"

//#include "Runtime/Function/Physics/physics.h"


void ApplicationBase::Init() {

	Debug::Init();
    DEBUG_LOG_INFO("game start");

    //LoadConfig();

    Time::Init();

    //初始化图形库，例如glfw
    InitGraphicsLibraryFramework();

    UpdateScreenSize();

    TypeManager::Initialize(new TypeManager());

    SerializerManager::Initialize(new SerializerManager());

    //初始化 fmod
    //Audio::Init();

    //初始化物理引擎
    // Physics::Init();
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

void ApplicationBase::Update(){
	Time::Update();
    UpdateScreenSize();

    GameObject::Foreach([](GameObject* game_object) {
        if (game_object->active()) {
            game_object->ForeachComponent([](Component* component) {
                component->Update();
                });
        }
        });

    Input::Update();
    //Audio::Update();
}


void ApplicationBase::Render(){
    //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
    Camera::Foreach([&]() {
        GameObject::Foreach([](GameObject* game_object) {
            if (game_object->active() == false) {
                return;
            }
            MeshRenderer* mesh_renderer = game_object->GetComponent<MeshRenderer>();
            if (mesh_renderer == nullptr) {
                return;
            }
            mesh_renderer->Render();
            });
        });
}

void ApplicationBase::FixedUpdate(){
    //Physics::FixedUpdate();

    GameObject::Foreach([](GameObject* game_object) {
        if (game_object->active()) {
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