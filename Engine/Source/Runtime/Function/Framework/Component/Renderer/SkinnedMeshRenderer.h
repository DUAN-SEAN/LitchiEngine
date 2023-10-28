
#pragma once

#include "MeshRenderer.h"
#include "Runtime/Function/Renderer/Resources/SkinnedData.h"

namespace LitchiRuntime
{
	class SkinnedMeshRenderer :public MeshRenderer
	{
	public:

		SkinnedMeshRenderer();
		~SkinnedMeshRenderer();

		void Update() override;
		/*void Render(RenderCamera* renderCamera, Matrix const* lightVPMat, Framebuffer4Depth* shadowMapFBO) override;
		void RenderShadowMap() override;*/

		std::vector<Matrix>& GetCurrentFinalTransformCacheArr()
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
		void CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets, std::vector<Matrix>& finalTransforms);

		/**
		 * \brief ���浱ǰ֡���������ձ任 (����*final = final Pos in Model Space)
		 */
		std::vector<Matrix> m_finalTransformCacheArr;
	};

}
