
#include "render_System.h"
#include "render_pipeline.h"

namespace LitchiRuntime
{
	RenderSystem* RenderSystem::instance_;

	void RenderSystem::Initialize(RenderSystem* instance, bool isUseEditor, bool isOutput2RT)
	{
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

	void RenderSystem::Render()
	{
		if(is_use_editor)
		{
			editor_render_pipeline_->Render(render_context_);
		}
		// game_render_pipeline_->Render(render_context_);// todo 先关掉Game管线
	}
}