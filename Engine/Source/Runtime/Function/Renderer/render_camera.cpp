
#include "render_camera.h"

#include <glad/glad.h>

#include "gtc/matrix_transform.hpp"

#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Function/Renderer/gpu_resource_mapper.h"
#include "Runtime/Function/Renderer/render_texture.h"

namespace LitchiRuntime
{
	RenderCamera::RenderCamera() :clear_color_(49.f / 255, 77.f / 255, 121.f / 255, 1.f), clear_flag_(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	{

	}
	void RenderCamera::SetView(const glm::vec3& cameraPos,const glm::vec3& centerPos, const glm::vec3& cameraUp) {
		view_mat4_ = glm::lookAt(cameraPos, centerPos, cameraUp);
	}

	void RenderCamera::SetProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip) {
		projection_mat4_ = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
	}

	void RenderCamera::Clear() {
		glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
		glClear(clear_flag_);
	}

	void RenderCamera::CheckRenderToTexture() {

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

	void RenderCamera::CheckCancelRenderToTexture() {
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

	void RenderCamera::set_target_render_texture(RenderTexture* render_texture) {
		if (render_texture == nullptr) {
			clear_target_render_texture();
		}
		target_render_texture_ = render_texture;
	}

	void RenderCamera::clear_target_render_texture() {
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
