#include "render_pipeline.h"

#include "render_pass.h"
#include "render_texture.h"

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
		if(useRT)
		{
			output_renderTexture_ = new RenderTexture();
			output_renderTexture_->Init(480, 320);
			output_renderTexture_->set_in_use(useRT);
		}

		color_render_pass_ = new ColorRenderPass();

	}
	void RenderPipeline::PreRender()
	{
	}

	void RenderPipeline::Render(RenderContext* render_context)
	{
		// ����ǰ����ӳ�ģʽ��Ⱦ��Ⱦ��ʽ

		// ��Ӱpass

		// ��ɫpass
		color_render_pass_->Render(render_context);

		// 
	}
}
