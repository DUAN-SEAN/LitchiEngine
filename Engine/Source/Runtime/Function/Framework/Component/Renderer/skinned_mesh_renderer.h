
#pragma once

#include "mesh_renderer.h"
#include "Runtime/Function/Renderer/Resources/SkinnedData.h"

namespace LitchiRuntime
{
	class SkinnedMeshRenderer :public MeshRenderer
	{
	public:

		SkinnedMeshRenderer();
		~SkinnedMeshRenderer();

		void Update() override;
		void Render(RenderCamera* renderCamera, glm::mat4 const* lightVPMat, Framebuffer4Depth* shadowMapFBO) override;
		void RenderShadowMap() override;

		std::vector<glm::mat4x4>& GetCurrentFinalTransformCacheArr()
		{
			return m_finalTransformCacheArr;
		}

	public:

		RTTR_ENABLE(MeshRenderer)

	private:

		/**
		* \brief ��������ڵ�ǰ֡�����ձ任
		* \param timePos ��ǰʱ��ƫ��
		* \param clip ����Ƭ��
		* \param boneHierarchy �����㼶����
		* \param boneOffsets T-Pose ����, ���ڽ�����ת���������ռ�
		* \param finalTransforms �����������ձ任����
		*/
		void CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<glm::mat4>& boneOffsets, std::vector<glm::mat4x4>& finalTransforms);

		/**
		 * \brief ���浱ǰ֡���������ձ任 (����*final = final Pos in Model Space)
		 */
		std::vector<glm::mat4x4> m_finalTransformCacheArr;
	};

}
