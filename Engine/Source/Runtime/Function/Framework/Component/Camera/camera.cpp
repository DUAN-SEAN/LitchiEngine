
#include "camera.h"
#include <memory>
#include <gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <rttr/registration>

#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Renderer/render_system.h"

namespace LitchiRuntime
{

	Camera::Camera() :depth_(0), culling_mask_(0x01) {
		
	}
	
	Camera::~Camera() {
	}

	void Camera::SetView(const glm::vec3& centerPos, const glm::vec3& cameraUp) {
		auto transform = game_object()->GetComponent<Transform>();
		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetView(transform->position(), centerPos, cameraUp);
	}

	void Camera::SetProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip) {

		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetProjection(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
	}
	
	void Camera::set_clear_color(float r, float g, float b, float a)
	{
		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->set_clear_color(r, g, b, a);
	}

	void Camera::set_clear_flag(unsigned int clear_flag)
	{
		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->set_clear_flag(clear_flag);
	}

	void Camera::set_depth(unsigned char depth) {
		if (depth_ == depth) {
			return;
		}
		depth_ = depth;
		// Sort();
	}

	//void Camera::Sort() {
	//	std::sort(all_camera_.begin(), all_camera_.end(), [](Camera* a, Camera* b) {
	//		return a->depth() < b->depth();
	//		});
	//}

	//void Camera::Foreach(std::function<void()> func) {
	//	for (auto iter = all_camera_.begin(); iter != all_camera_.end(); iter++) {
	//		current_camera_ = *iter;
	//		// current_camera_->CheckRenderToTexture();
	//		current_camera_->Clear();
	//		func();
	//		//current_camera_->CheckCancelRenderToTexture();
	//	}
	//}

}











