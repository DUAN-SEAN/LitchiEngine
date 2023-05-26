
#include <glad/glad.h>

#include "gpu_resource_mapper.h"
#include "render_texture.h"

#include "Resources/texture2d.h"
#include "Runtime/Core/Log/debug.h"

namespace LitchiRuntime
{
	RenderTexture::RenderTexture() : width_(128), height_(128), frame_buffer_object_handle_(0), in_use_(false),
		color_texture_2d_(nullptr), depth_texture_2d_(nullptr) {
	}

	RenderTexture::~RenderTexture() {
		if (frame_buffer_object_handle_ > 0) {
			GLuint frame_buffer_object_id = GPUResourceMapper::GetFBO(frame_buffer_object_handle_);
			glDeleteFramebuffers(1, &frame_buffer_object_id);
		}
		//删除Texture2D
		if (color_texture_2d_ != nullptr) {
			delete color_texture_2d_;
		}
		if (depth_texture_2d_ != nullptr) {
			delete depth_texture_2d_;
		}
	}

	void RenderTexture::Init(unsigned short width, unsigned short height) {
		width_ = width;
		height_ = height;
		color_texture_2d_ = Texture2D::Create(width_, height_, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, nullptr, 0);
		depth_texture_2d_ = Texture2D::Create(width_, height_, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr, 0);
		//创建FBO任务
		frame_buffer_object_handle_ = GPUResourceMapper::GenerateFBOHandle();

		//查询当前GL实现所支持的最大的RenderBufferSize,就是尺寸
		GLint support_size = 0;
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &support_size);
		//如果我们设定的尺寸超过了所支持的尺寸，就抛出错误
		if (support_size <= width || support_size <= height) {
			DEBUG_LOG_ERROR("CreateFBO FBO Size Too Large!Not Support!");
			return;
		}
		//创建FBO
		GLuint frame_buffer_object_id = 0;
		glGenFramebuffers(1, &frame_buffer_object_id); __CHECK_GL_ERROR__
			if (frame_buffer_object_id == 0) {
				DEBUG_LOG_ERROR("CreateFBO FBO Error!");
				return;
			}
		GPUResourceMapper::MapFBO(frame_buffer_object_handle_, frame_buffer_object_id);

		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object_id); __CHECK_GL_ERROR__

		//将颜色纹理绑定到FBO颜色附着点
		GLuint color_texture = GPUResourceMapper::GetTexture(color_texture_2d_->texture_handle());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0); __CHECK_GL_ERROR__

		//将深度纹理绑定到FBO深度附着点
		GLuint depth_texture = GPUResourceMapper::GetTexture(depth_texture_2d_->texture_handle());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0); __CHECK_GL_ERROR__

		glBindFramebuffer(GL_FRAMEBUFFER, 0); __CHECK_GL_ERROR__
	}

	void RenderTexture::UpdateRT(unsigned short width, unsigned short height)
	{
		color_texture_2d_->UpdateImage(0, 0, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, nullptr, 0);
		depth_texture_2d_->UpdateImage(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr, 0);
	}

}

