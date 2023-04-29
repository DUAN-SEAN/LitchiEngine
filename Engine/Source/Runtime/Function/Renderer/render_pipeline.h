#pragma once

namespace LitchiRuntime
{
	class RenderTexture;
	class Scene;
	class RenderCamera;
	class RenderPass;
	class ColorRenderPass;
	class RenderContext;

	/**
	 * \brief 渲染管线,会按照顺序渲染多个Pass
	 * 管线的结果将输出到FBO（默认的屏幕FBO或指定的FBO）
	 */
	class RenderPipeline
	{
	public:
		RenderPipeline();
		~RenderPipeline();

		void Initialize(bool useRT);

		/**
		 * \brief 渲染前执行
		 */
		void PreRender();

		/**
		 * \brief 渲染
		 */
		void Render(RenderContext* render_context);

		/**
		 * \brief 获取RT, 如果设置管线的最终目标是
		 * \return RT
		 */
		RenderTexture* GetOutputRT() { return output_renderTexture_;}

	private:
		/**
		 * \brief 缓存渲染管线,暂时只有一个
		 */
		ColorRenderPass* color_render_pass_;

		/**
		 * \brief 输出RT
		 */
		RenderTexture* output_renderTexture_;

	};
}
