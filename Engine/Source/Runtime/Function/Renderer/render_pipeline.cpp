
#include <glad/glad.h>
#include "render_pipeline.h"

#include "gpu_resource_mapper.h"
#include "render_camera.h"
#include "render_pass.h"
#include "render_System.h"
#include "render_texture.h"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Screen/screen.h"

namespace LitchiRuntime
{
	RenderPipeline::RenderPipeline():output_renderTexture_(nullptr)
	{
	}

	RenderPipeline::~RenderPipeline()
	{
		if(output_renderTexture_ != nullptr)
		{
			delete output_renderTexture_;
		}
		delete color_render_pass_;
	}

	void RenderPipeline::Initialize(bool useRT)
	{
		is_use_rt = useRT;
		if(useRT)
		{
			output_renderTexture_ = new RenderTexture();
			output_renderTexture_->Init(480, 320);
			output_renderTexture_->set_in_use(useRT);
		}

		color_render_pass_ = new ColorRenderPass();

	}
	void RenderPipeline::PreRender(RenderContext* render_context)
	{
		// 设置相机参数
		// 计算主相机的projection matrix
		// 计算相机的vp矩阵
		render_context->main_render_camera_->SetAspectRatio(render_context->width_ / render_context->height_);
		render_context->main_render_camera_->UpdateProjection();

		if(is_use_rt)
		{
			// 设置output texture
			output_renderTexture_->set_width(render_context->width_);
			output_renderTexture_->set_height(render_context->height_);

			// 将RT的尺寸设置为当前屏幕的尺寸
			output_renderTexture_->UpdateRT(render_context->width_, render_context->height_);
		}
	}

	void RenderPipeline::Render(RenderContext* render_context)
	{
		// 根据前向和延迟模式渲染渲染方式

		// todo 先暂时在这里设置最终输出的RT
		if(is_use_rt)
		{
			GLuint frame_buffer_object_id = GPUResourceMapper::GetFBO(output_renderTexture_->frame_buffer_object_handle());
			glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object_id); __CHECK_GL_ERROR__
			glViewport(0, 0, render_context->width_, render_context->height_);

			//检测帧缓冲区完整性，如果完整的话就开始进行绘制
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); __CHECK_GL_ERROR__
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} ! fbo_id:{}", status, frame_buffer_object_id);
				return;
			}
		}

		// 阴影pass

		// 颜色pass
		color_render_pass_->Render(render_context);

		//

		if(is_use_rt)
		{
			//更新ViewPort的尺寸
			glViewport(0, 0, Screen::width(), Screen::height());

			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE); __CHECK_GL_ERROR__

		}
	}
}
