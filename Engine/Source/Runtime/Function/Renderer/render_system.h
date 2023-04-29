#pragma once

namespace LitchiRuntime
{
	class RenderPipeline;
	class Scene;
	class RenderCamera;

	/**
	 * \brief ��Ⱦ������,������Ⱦ�����������Ϣ��Դ��Context
	 * todo:�Ժ����Բ�ͬ�����ṩ��ͬ����Ⱦ������
	 */
	class RenderContext
	{
	public:
		/**
		 * \brief ����Ⱦ������
		 * ÿ�����߿ɽ���һ�������,��������д��ڶ�������,��ôֻ��һ���������Ч
		 */
		RenderCamera* main_render_camera_;

		float width_;
		float height_;

		/**
		 * \brief �������
		 */
		Scene* scene_;
		// more 
	};

	/**
	 * \brief ��Ⱦϵͳ����
	 * Ŀǰ������������
	 *	Editor:��Ⱦ�༭��ģʽ�µĳ���
	 *	Game:��Ⱦ��Ϸģʽ�µĳ���
	 * todo:�Ժ������������ߣ���鿴ģ�͡������ȹ���
	 */
	class RenderSystem {
	public:

		RenderSystem();
		~RenderSystem();

		// ��Ⱦ���е���Ⱦ����
		void Render();

		/**
		 * \brief ��ȡ��Ⱦ������
		 * \return renderContext
		 */
		RenderContext* GetRenderContext() { return render_context_; }

		RenderPipeline* GetEditorRenderPipeline() { return editor_render_pipeline_; }
		RenderPipeline* GetGameRenderPipeline() { return game_render_pipeline_; }

		/**
		 * \brief
		 * \param instance system���
		 * \param isUseEditor �Ƿ�ʹ��Editor
		 * \param isOutput2RT �Ƿ������RT
		 */
		static void Initialize(RenderSystem* instance, bool isUseEditor, bool isOutput2RT);

		static RenderSystem* Instance() { return instance_; }

	private:

		static RenderSystem* instance_;

		RenderContext* render_context_;

		bool is_use_editor;

		// �༭�� ��Ⱦ����
		RenderPipeline* editor_render_pipeline_;

		// game ��Ⱦ����
		RenderPipeline* game_render_pipeline_;
	};
}
