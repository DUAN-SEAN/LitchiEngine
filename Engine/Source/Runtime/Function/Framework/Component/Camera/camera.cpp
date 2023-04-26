
#include "camera.h"
#include <memory>
#include <gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <rttr/registration>

#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Renderer/gpu_resource_mapper.h"
#include "Runtime/Function/Renderer/render_texture.h"

namespace LitchiRuntime
{
	std::vector<Camera*> Camera::all_camera_;
	Camera* Camera::current_camera_;

	Camera::Camera() :clear_color_(49.f / 255, 77.f / 255, 121.f / 255, 1.f), clear_flag_(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT), depth_(0), culling_mask_(0x01) {
		//默认获取现有Camera最大depth，设置当前Camera.depth +1
		if (all_camera_.size() > 0) {
			unsigned char max_depth = all_camera_.back()->depth();
			depth_ = max_depth + 1;
		}
		all_camera_.push_back(this);
	}


	Camera::~Camera() {
		auto iter = std::find(all_camera_.begin(), all_camera_.end(), this);
		if (iter != all_camera_.end()) {
			all_camera_.erase(iter);
		}
	}

	void Camera::SetView(const glm::vec3& centerPos, const glm::vec3& cameraUp) {
		auto transform = game_object()->GetComponent<Transform>();
		view_mat4_ = glm::lookAt(transform->position(), centerPos, cameraUp);
	}

	void Camera::SetProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip) {
		projection_mat4_ = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
	}

	void Camera::Clear() {
		glClear(clear_flag_);
		glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
	}

	void Camera::set_depth(unsigned char depth) {
		if (depth_ == depth) {
			return;
		}
		depth_ = depth;
		Sort();
	}

	void Camera::Sort() {
		std::sort(all_camera_.begin(), all_camera_.end(), [](Camera* a, Camera* b) {
			return a->depth() < b->depth();
			});
	}

	void Camera::Foreach(std::function<void()> func) {
		for (auto iter = all_camera_.begin(); iter != all_camera_.end(); iter++) {
			current_camera_ = *iter;
			/*current_camera_->CheckRenderToTexture();
			current_camera_->Clear();*/
			func();
			//current_camera_->CheckCancelRenderToTexture();
		}
	}


	void Camera::CheckRenderToTexture() {
		if (target_render_texture_ == nullptr) {//没有设置目标RenderTexture
			return;
		}
		if (target_render_texture_->in_use()) {
			return;
		}
		if (target_render_texture_->frame_buffer_object_handle() == 0) {//还没有初始化，没有生成FBO。
			return;
		}

		glViewport(0, 0, target_render_texture_->width(), target_render_texture_->height());

		GLuint frame_buffer_object_id = GPUResourceMapper::GetFBO(target_render_texture_->frame_buffer_object_handle());
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object_id); __CHECK_GL_ERROR__
			//检测帧缓冲区完整性，如果完整的话就开始进行绘制
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); __CHECK_GL_ERROR__
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} !", status);
				return;
			}
		target_render_texture_->set_in_use(true);
	}

	void Camera::CheckCancelRenderToTexture() {
		if (target_render_texture_ == nullptr) {//没有设置目标RenderTexture
			return;
		}
		if (target_render_texture_->in_use() == false) {
			return;
		}
		if (target_render_texture_->frame_buffer_object_handle() == 0) {//还没有初始化，没有生成FBO。
			return;
		}
		//更新ViewPort的尺寸
		glViewport(0, 0, Screen::width(), Screen::height());

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE); __CHECK_GL_ERROR__

		target_render_texture_->set_in_use(false);
	}

	void Camera::set_target_render_texture(RenderTexture* render_texture) {
		if (render_texture == nullptr) {
			clear_target_render_texture();
		}
		target_render_texture_ = render_texture;
	}

	void Camera::clear_target_render_texture() {
		if (target_render_texture_ == nullptr) {//没有设置目标RenderTexture
			return;
		}
		if (target_render_texture_->in_use() == false) {
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE); __CHECK_GL_ERROR__

		target_render_texture_->set_in_use(false);
	}

}











