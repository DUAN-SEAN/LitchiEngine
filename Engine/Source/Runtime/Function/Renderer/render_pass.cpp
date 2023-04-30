#include "render_pass.h"

#include <glad/glad.h>
#include "render_system.h"
#include "render_camera.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Scene/scene_manager.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

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

		// 获取当前需要渲染的相机 和 场景
		RenderCamera* render_camera = render_context->main_render_camera_;
		Scene* scene = render_context->scene_;

		glViewport(0, 0, render_context->width_, render_context->width_);
		render_camera->Clear();

		// 遍历所有的物体,执行MeshRenderer的Render函数
		scene->Foreach([&](GameObject* game_object) {
			if (game_object->active()) {
				game_object->ForeachComponent([&](Component* component) {
					auto* mesh_renderer = dynamic_cast<MeshRenderer*>(component);
					if (mesh_renderer == nullptr) {
						return;
					}
					mesh_renderer->Render(render_camera);
					});
			}
			});
	}

}
