
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
		* \brief 计算骨骼在当前帧的最终变换
		* \param timePos 当前时间偏移
		* \param clip 动画片段
		* \param boneHierarchy 骨骼层级索引
		* \param boneOffsets T-Pose 矩阵, 用于将顶点转换到骨骼空间
		* \param finalTransforms 输出顶点的最终变换矩阵
		*/
		void CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets, std::vector<Matrix>& finalTransforms);

		/**
		 * \brief 缓存当前帧骨骼的最终变换 (顶点*final = final Pos in Model Space)
		 */
		std::vector<Matrix> m_finalTransformCacheArr;
	};

}
