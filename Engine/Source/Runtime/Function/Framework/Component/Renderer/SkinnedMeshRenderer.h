
#pragma once

#include "MeshRenderer.h"
#include "Runtime/Function/Renderer/Resources/SkinnedData.h"

namespace LitchiRuntime
{
	class SkinnedMeshRenderer : public MeshRenderer
	{
	public:

		SkinnedMeshRenderer();
		~SkinnedMeshRenderer();

		void Update() override;
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
		void CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets);

		void CreateBoneBuffer();

		bool m_isDirty = true;

		Cb_Bone_Arr m_bone_arr;
		std::shared_ptr<RHI_ConstantBuffer> m_bone_constant_buffer;
	};

}
