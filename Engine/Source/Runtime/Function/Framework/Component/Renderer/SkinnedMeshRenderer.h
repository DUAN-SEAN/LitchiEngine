
#pragma once

#include "MeshRenderer.h"

namespace LitchiRuntime
{
	class SkinnedMeshRenderer : public MeshRenderer
	{
	public:

		SkinnedMeshRenderer();
		~SkinnedMeshRenderer();

		void Awake() override;

		void Update() override;

		void PostResourceLoaded() override;
		void PostResourceModify() override;

		std::shared_ptr<RHI_ConstantBuffer> GetBoneConstantBuffer() { return m_bone_constant_buffer; }

	public:

		RTTR_ENABLE(MeshRenderer)

	private:
		void CreateDefaultBoneBuffer();
		void CalcDefaultFinalTransform(std::vector<int>& boneHierarchy, std::vector<Matrix>& nodelDefaultTransforms);

		/**
		* \brief ��������ڵ�ǰ֡�����ձ任
		* \param timePos ��ǰʱ��ƫ��
		* \param clip ����Ƭ��
		* \param boneHierarchy �����㼶����
		* \param boneOffsets T-Pose ����, ���ڽ�����ת���������ռ�
		* \param finalTransforms �����������ձ任����
		*/
		void CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets);

		void CreateBoneBuffer();

		bool m_isDirty = true;
		bool m_isFirstTick = true;

		Cb_Bone_Arr m_bone_arr;
		std::shared_ptr<RHI_ConstantBuffer> m_bone_constant_buffer;
	};

}
