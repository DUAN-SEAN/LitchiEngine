//
// Created by captain on 2021/7/10.
//

#include "Editor/include/login_scene.h"
#include <rttr/registration>

#include <glad/glad.h>
#include <glm.hpp>
#include <ext.hpp>
#include <gtx/transform2.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/euler_angles.hpp>

#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Renderer/material.h"
#include "Runtime/Function/Input/input.h"
#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Function/Input/key_code.h"



using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<LoginScene>("LoginScene")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

LoginScene::LoginScene() :Component()
{

}

void LoginScene::Awake() {
    //创建模型 GameObject
    GameObject* go=new GameObject("fishsoup_pot");
    go->set_layer(0x01);

    //挂上 Transform 组件
    transform_fishsoup_pot_=dynamic_cast<Transform*>(go->AddComponent<Transform>());

    //挂上 MeshFilter 组件
    auto mesh_filter=dynamic_cast<MeshFilter*>(go->AddComponent<MeshFilter>());
    mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

    //挂上 MeshRenderer 组件
    auto mesh_renderer=dynamic_cast<MeshRenderer*>(go->AddComponent<MeshRenderer>());
    Material* material=new Material();//设置材质
    material->Parse("material/fishsoup_pot.mat");
    mesh_renderer->SetMaterial(material);

    //创建相机2 GameObject
    auto go_camera_2=new GameObject("main_camera");
    //挂上 Transform 组件
    transform_camera_2_=dynamic_cast<Transform*>(go_camera_2->AddComponent<Transform>());
    transform_camera_2_->set_position(glm::vec3(1, 0, 10));
    //挂上 Camera 组件
    camera_2_=dynamic_cast<Camera*>(go_camera_2->AddComponent<Camera>());
    //第二个相机不能清除之前的颜色。不然用第一个相机矩阵渲染的物体就被清除 没了。
    camera_2_->set_clear_flag(GL_DEPTH_BUFFER_BIT);
    camera_2_->set_depth(1);
    camera_2_->set_culling_mask(0x02);


    //创建相机1 GameObject
    auto go_camera_1=new GameObject("main_camera");
    //挂上 Transform 组件
    transform_camera_1_=dynamic_cast<Transform*>(go_camera_1->AddComponent<Transform>());
    transform_camera_1_->set_position(glm::vec3(0, 0, 10));
    //挂上 Camera 组件
    camera_1_=dynamic_cast<Camera*>(go_camera_1->AddComponent<Camera>());
    camera_1_->set_depth(0);


    last_frame_mouse_position_=Input::mousePosition();
}

void LoginScene::Update() {
    camera_2_->SetView(glm::vec3(transform_camera_2_->position().x, 0, 0), glm::vec3(0, 1, 0));
    camera_2_->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);

    camera_1_->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera_1_->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);

    //旋转物体
    if(Input::GetKeyDown(KEY_CODE_R)){
        static float rotate_eulerAngle=0.f;
        rotate_eulerAngle+=0.1f;
        glm::quat rotation=transform_fishsoup_pot_->rotation();

        // todo 旋转物体
        rotation.y=rotate_eulerAngle;
        transform_fishsoup_pot_->set_rotation(rotation);
    }

    //旋转相机
    if(Input::GetKeyDown(KEY_CODE_LEFT_ALT) && Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
        float degrees= Input::mousePosition().x - last_frame_mouse_position_.x;

        glm::mat4 old_mat4=glm::mat4(1.0f);
//        std::cout<<glm::to_string(old_mat4)<<std::endl;

        glm::mat4 rotate_mat4=glm::rotate(old_mat4,glm::radians(degrees),glm::vec3(0.0f,1.0f,0.0f));//以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。
        glm::vec4 old_pos=glm::vec4(transform_camera_1_->position(),1.0f);
        glm::vec4 new_pos=rotate_mat4*old_pos;//旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
        std::cout<<glm::to_string(new_pos)<<std::endl;

        transform_camera_1_->set_position(glm::vec3(new_pos));
    }
    last_frame_mouse_position_=Input::mousePosition();

    //鼠标滚轮控制相机远近
    transform_camera_1_->set_position(transform_camera_1_->position() *(10.0f - Input::mouse_scroll())/10.f);
}
