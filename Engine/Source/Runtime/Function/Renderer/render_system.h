#pragma once

namespace LitchiRuntime
{
	class RenderPipeline;
	class Scene;
	class RenderCamera;

	/**
	 * \brief 渲染上下文,所有渲染管线所需的信息来源于Context
	 * todo:以后会针对不同管线提供不同的渲染上下文
	 */
	class RenderContext
	{
	public:
		/**
		 * \brief 主渲染相机句柄
		 * 每个管线可接收一个主相机,如果场景中存在多个主相机,那么只有一个主相机生效
		 */
		RenderCamera* main_render_camera_;

		/**
		 * \brief 当前需要渲染的目标 宽高
		 */
		float width_, height_;

		/**
		 * \brief 场景句柄
		 */
		Scene* scene_;
		// more 
	};

	/**
	 * \brief 渲染系统定义
	 * 目前存在两个管线
	 *	Editor:渲染编辑器模式下的场景
	 *	Game:渲染游戏模式下的场景
	 * todo:以后将新增其他管线，如查看模型、动画等管线
	 */
	class RenderSystem {

	public:

		RenderSystem();
		~RenderSystem();

		void InitRenderContext(RenderCamera* renderCamera,float width,float height,Scene* scene);

		// 渲染所有的渲染管线
		void Render();

		/**
		 * \brief 获取渲染上下文
		 * \return renderContext
		 */
		RenderContext* GetRenderContext() { return render_context_; }

		RenderPipeline* GetEditorRenderPipeline() { return editor_render_pipeline_; }
		RenderPipeline* GetGameRenderPipeline() { return game_render_pipeline_; }

	public:

		/**
		 * \brief
		 * \param instance system句柄
		 * \param isUseEditor 是否使用Editor
		 * \param isOutput2RT 是否输出到RT
		 */
		static void Initialize(RenderSystem* instance, bool isUseEditor, bool isOutput2RT);

		static RenderSystem* Instance() { return instance_; }

	private:

		RenderContext* render_context_;

		bool is_use_editor;

		// 编辑器 渲染管线
		RenderPipeline* editor_render_pipeline_;

		// game 渲染管线
		RenderPipeline* game_render_pipeline_;

	private:

		static RenderSystem* instance_;

	};
}
