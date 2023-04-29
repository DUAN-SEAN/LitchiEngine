#pragma once

namespace LitchiRuntime
{
	class RenderCamera;
	class RenderContext;
	/**
	 * \brief ��ȾPass
	 * ����һ����������Ⱦ����
	 * �������Ⱦ ����͸�� ��͸����
	 * UI��Ⱦ
	 */
	class RenderPass
	{
	public:
		virtual void Initialize();

		virtual void Render(RenderContext* render_context);

	private:

	};

	/**
	 * \brief ������ɫ��Pass
	 */
	class ColorRenderPass :public RenderPass
	{
	public:
		void Render(RenderContext* render_context) override;
	private:
	};

}
