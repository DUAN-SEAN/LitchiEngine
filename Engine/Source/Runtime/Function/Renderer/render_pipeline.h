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
	 * \brief ��Ⱦ����,�ᰴ��˳����Ⱦ���Pass
	 * ���ߵĽ���������FBO��Ĭ�ϵ���ĻFBO��ָ����FBO��
	 */
	class RenderPipeline
	{
	public:
		RenderPipeline();
		~RenderPipeline();

		void Initialize(bool useRT);

		/**
		 * \brief ��Ⱦǰִ��
		 */
		void PreRender();

		/**
		 * \brief ��Ⱦ
		 */
		void Render(RenderContext* render_context);

		/**
		 * \brief ��ȡRT, ������ù��ߵ�����Ŀ����
		 * \return RT
		 */
		RenderTexture* GetOutputRT() { return output_renderTexture_;}

	private:
		/**
		 * \brief ������Ⱦ����,��ʱֻ��һ��
		 */
		ColorRenderPass* color_render_pass_;

		/**
		 * \brief ���RT
		 */
		RenderTexture* output_renderTexture_;

	};
}
