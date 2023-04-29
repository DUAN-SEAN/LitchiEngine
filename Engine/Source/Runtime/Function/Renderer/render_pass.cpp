#include "render_pass.h"
#include "render_pipeline.h"
#include "render_system.h"
#include "render_camera.h"
#include "Runtime/Function/Scene/scene_manager.h"

namespace LitchiRuntime
{
	void RenderPass::Initialize()
	{
	}

	void RenderPass::Render(RenderContext* render_context)
	{
	}


	void ColorRenderPass::Render(RenderContext* render_context)
	{
		RenderPass::Render(render_context);

		// 获取当前需要渲染的相机
		RenderCamera* render_camera = render_context->main_render_camera_;
		Scene* scene = render_context->scene_;


		
	}

}
