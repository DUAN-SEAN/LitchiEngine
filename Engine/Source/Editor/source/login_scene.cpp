//
// Created by captain on 2021/7/10.
//

#include "Editor/include/login_scene.h"
#include <rttr/registration>

#include <ext.hpp>
#include <gtc/quaternion.hpp>

#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Renderer/material.h"
#include "Runtime/Function/Input/input.h"
#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Input/key_code.h"
#include "Runtime/Function/Renderer/render_camera.h"
#include "Runtime/Function/Renderer/render_System.h"
#include "Runtime/Function/Renderer/render_texture.h"
#include "Runtime/Function/Scene/scene_manager.h"

using namespace rttr;
using namespace LitchiRuntime;

namespace LitchiEditor
{
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
		GameObject* go = new GameObject("fishsoup_pot",this->game_object()->GetScene());
		go->set_layer(0x01);

		//挂上 Transform 组件
		transform_fishsoup_pot_ = go->AddComponent<Transform>();

		//挂上 MeshFilter 组件
		auto mesh_filter = go->AddComponent<MeshFilter>();
		mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

		//挂上 MeshRenderer 组件
		auto mesh_renderer = go->AddComponent<MeshRenderer>();
		Material* material = new Material();//设置材质
		material->Parse("material/materialTemplete2.mat");
		//material->Parse("material/fishsoup_pot.mat");
		mesh_renderer->SetMaterial(material);
		DEBUG_LOG_INFO("LoadMaterial Done");

		transform_fishsoup_pot_->set_position(glm::vec3(0, 0, 0));

		////创建相机2 GameObject
		//auto go_camera_2=new GameObject("main_camera");
		////挂上 Transform 组件
		//transform_camera_2_=dynamic_cast<Transform*>(go_camera_2->AddComponent<Transform>());
		//transform_camera_2_->set_position(glm::vec3(1, 0, 10));
		////挂上 Camera 组件
		//camera_2_=dynamic_cast<Camera*>(go_camera_2->AddComponent<Camera>());
		////第二个相机不能清除之前的颜色。不然用第一个相机矩阵渲染的物体就被清除 没了。
		//camera_2_->set_clear_flag(GL_DEPTH_BUFFER_BIT);
		//camera_2_->set_depth(1);
		//camera_2_->set_culling_mask(0x02);


		////创建相机1 GameObject
		//auto go_camera_1 = new GameObject("main_camera", this->game_object()->GetScene());
		////挂上 Transform 组件
		//transform_camera_1_ = go_camera_1->AddComponent<Transform>();
		//transform_camera_1_->set_position(glm::vec3(0, 0, 10));
		////挂上 Camera 组件
		//camera_1_ = go_camera_1->AddComponent<Camera>();
		//camera_1_->set_depth(0);


		//camera_1_->SetView((glm::vec3(0,0,0)), (glm::vec3(0, 1, 0)));
		//// camera_1_->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);
		//camera_1_->SetProjection(60.f, 480/320.0, 1.f, 1000.f);


		/*auto rt = new RenderTexture();
		rt->set_in_use(true);
		rt->set_width(400);
		rt->set_height(600);
		camera_1_->set_target_render_texture(rt);*/


		last_frame_mouse_position_ = Input::mousePosition();
	}

	void LoginScene::Update() {


		auto go = SceneManager::GetScene("DefaultScene")->Find("fishsoup_pot");
		auto trans = go->GetComponent<Transform>();

		float speedMove = Time::delta_time() * 10.0f;
		glm::vec3 moveDir = glm::vec3(0.0f, 0.0f, 0.0f);
		if (Input::GetKeyDown(KEY_CODE_W))
		{
			moveDir.z = -1.0f;
		}
		else if (Input::GetKeyDown(KEY_CODE_S))
		{
			moveDir.z = 1.0f;
		}
		if (Input::GetKeyDown(KEY_CODE_A))
		{
			moveDir.x = -1.0f;
		}
		else if (Input::GetKeyDown(KEY_CODE_D))
		{
			moveDir.x = 1.0f;
		}
		glm::vec3 newPos = trans->position() + moveDir * speedMove;
		trans->set_position(newPos);

		if(Input::GetKeyDown(KEY_CODE_U))
		{
			RenderSystem::Instance()->GetRenderContext()->width_ = 800;
			RenderSystem::Instance()->GetRenderContext()->height_ = 600;

			RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetAspectRatio(800 / 600);
		}

		if(Input::GetKeyDown(KEY_CODE_J))
		{

			RenderSystem::Instance()->GetRenderContext()->width_ = 480;
			RenderSystem::Instance()->GetRenderContext()->height_ = 320;

			RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetAspectRatio(480 / 320);
		}


		return;

		/* camera_2_->SetView(glm::vec3(transform_camera_2_->position().x, 0, 0), glm::vec3(0, 1, 0));
		 camera_2_->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);*/
		
		float speedRotation = Time::delta_time() * 1.0f;
		//旋转物体
		if (Input::GetKeyDown(KEY_CODE_R)) {
			static float rotate_eulerAngle = 0.f;
			rotate_eulerAngle += 0.1f * Time::delta_time();

			glm::quat rotation = transform_fishsoup_pot_->rotation();

			auto r1 = glm::quat(glm::vec3(0.0f, rotate_eulerAngle, 0.0f));

			// todo 旋转物体
			rotation = rotation * r1;
			transform_fishsoup_pot_->set_rotation(rotation);
		}

		// 平移相机
		glm::vec3 cameraMoveDir = glm::vec3(0.0f, 0.0f, 0.0f);
		if (Input::GetKeyDown(KEY_CODE_W))
		{
			cameraMoveDir.z = -1.0f;
		}
		else if (Input::GetKeyDown(KEY_CODE_S))
		{
			cameraMoveDir.z = 1.0f;
		}
		if (Input::GetKeyDown(KEY_CODE_A))
		{
			cameraMoveDir.x = -1.0f;
		}
		else if (Input::GetKeyDown(KEY_CODE_D))
		{
			cameraMoveDir.x = 1.0f;
		}

		glm::vec3 camerNewPos = transform_camera_1_->position() + cameraMoveDir * speedMove;
		transform_camera_1_->set_position(camerNewPos);


		////旋转相机
		//if(Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
		//    float degrees= Input::mousePosition().x - last_frame_mouse_position_.x;
		//    float degreesY= Input::mousePosition().y - last_frame_mouse_position_.y;
		//    glm::mat4 old_mat4=glm::mat4(1.0f);
		//	//        std::cout<<glm::to_string(old_mat4)<<std::endl;
		//    glm::mat4 rotate_mat4=glm::rotate(old_mat4,glm::radians(degrees),glm::vec3(0.0f,1.0f,0.0f));//以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。
		//    rotate_mat4 = glm::rotate(rotate_mat4, glm::radians(degreesY), glm::vec3(1.0f, 0.0f, 0.0f));
		//    glm::vec4 old_pos=glm::vec4(transform_camera_1_->position(),1.0f);
		//    glm::vec4 new_pos=rotate_mat4*old_pos;//旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
		//    std::cout<<glm::to_string(new_pos)<<std::endl;
		//    transform_camera_1_->set_position(glm::vec3(new_pos));
		//    glm::vec3 cameraOriginCenter = glm::vec3(0, 0, -1);
		//}

		// 旋转相机
		if (Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			float degrees = Input::mousePosition().x - last_frame_mouse_position_.x;
			float degreesY = Input::mousePosition().y - last_frame_mouse_position_.y;

			auto curCameraRotation = transform_camera_1_->rotation();

			glm::quat normalQuat;
			auto rotationX = glm::rotate(normalQuat, degrees * speedRotation, glm::vec3(0, 1, 0));
			auto rotationY = glm::rotate(normalQuat, degreesY * speedRotation, glm::vec3(1, 0, 0));

			auto newRotation = rotationX * rotationY * curCameraRotation;

			transform_camera_1_->set_rotation(newRotation);

		}


		if (Input::GetKeyDown(KEY_CODE_SPACE))
		{
			float degrees = 4.0f;

			auto curCameraRotation = transform_camera_1_->rotation();

			glm::quat normalQuat;
			auto rotationZ = glm::rotate(normalQuat, degrees * speedRotation, glm::vec3(0, 0, 1));

			auto newRotation = rotationZ * curCameraRotation;

			transform_camera_1_->set_rotation(newRotation);
		}

		glm::vec4 cameraOriginCenter = glm::vec4(0, 0, -1, 1);
		auto newCenterPos = transform_camera_1_->toWorldMatrix() * cameraOriginCenter;


		glm::vec4 up = glm::vec4(0, 1, 0, 0);
		auto newUp = transform_camera_1_->toWorldMatrix() * up;

		//camera_1_->SetView((glm::vec3(newCenterPos.x, newCenterPos.y, newCenterPos.z)), (glm::vec3(newUp.x, newUp.y, newUp.z)));
		//camera_1_->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);


		last_frame_mouse_position_ = Input::mousePosition();

		//鼠标滚轮控制相机远近
		transform_camera_1_->set_position(transform_camera_1_->position() * (10.0f - Input::mouse_scroll()) / 10.f);
	}

}
