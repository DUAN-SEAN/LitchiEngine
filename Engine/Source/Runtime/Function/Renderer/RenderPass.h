#pragma once

namespace LitchiRuntime
{
	class RenderCamera;
	class RenderContext;
	/**
	 * \brief 渲染Pass
	 * 进行一次完整的渲染管线
	 * 主相机渲染 （不透明 半透明）
	 * UI渲染
	 */
	class RenderPass
	{
	public:
		virtual void Initialize();

		virtual void Render(RenderContext* render_context);

	private:

	};

	/**
	 * \brief 绘制颜色的Pass
	 */
	class ColorRenderPass :public RenderPass
	{
	public:
		void Render(RenderContext* render_context) override;
	private:
	};

	class ShadowRenderPass :public RenderPass
	{
	public:
		 
	};

	/**
	 * \brief 执行前向渲染的pass
	 */
	class ForwardRenderPass :public RenderPass
	{
	public:
		virtual void Render(RenderContext* render_context) override;

		// 输入是 相机、物体和阴影数据
		// 输出是 帧缓冲
	};

	class UIRenderPass :public RenderPass
	{

	};

}
