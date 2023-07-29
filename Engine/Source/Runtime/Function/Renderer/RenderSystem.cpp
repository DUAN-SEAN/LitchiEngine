
#include "RenderSystem.h"
#include "RenderPipeline.h"

namespace LitchiRuntime
{
	RenderSystem* RenderSystem::instance_;

	void RenderSystem::Initialize(RenderSystem* instance, bool isUseEditor, bool isOutput2RT)
	{
		instance_ = instance;
		instance_->is_use_editor = isUseEditor;
		instance_->editor_render_pipeline_->Initialize(isOutput2RT);
		instance_->game_render_pipeline_->Initialize(isOutput2RT);
	}

	RenderSystem::RenderSystem()
	{
		render_context_ = new RenderContext();
		editor_render_pipeline_ = new RenderPipeline();
		game_render_pipeline_ = new RenderPipeline();
	}

	RenderSystem::~RenderSystem()
	{
		delete game_render_pipeline_;
		delete editor_render_pipeline_;
		delete render_context_;
	}

	void RenderSystem::InitRenderContext(RenderCamera* renderCamera, float width, float height, Scene* scene)
	{
		render_context_->main_render_camera_ = renderCamera;
		render_context_->width_ = width;
		render_context_->height_ = height;
		render_context_->scene_ = scene;
	}

	void RenderSystem::Render()
	{
		if(is_use_editor)
		{
			editor_render_pipeline_->PreRender(render_context_);
			editor_render_pipeline_->Render(render_context_);
		}
		// game_render_pipeline_->Render(render_context_);// todo 先关掉Game管线
	}
}