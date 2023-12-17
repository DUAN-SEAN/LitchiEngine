
#include "SkinnedMeshRenderer.h"
#include "MeshFilter.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RHI/RHI_ConstantBuffer.h"

using namespace LitchiRuntime;

LitchiRuntime::SkinnedMeshRenderer::SkinnedMeshRenderer()
{
}

LitchiRuntime::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

void SkinnedMeshRenderer::Update()
{
	MeshRenderer::Update();

	if (m_isFirstTick && m_material!= nullptr)
	{
		m_material->GetShader()->ChangeVertexType(RHI_Vertex_Type::PosUvNorTanBone);

		m_isFirstTick = false;
	}

	// ͨ��MeshFilter��ȡ��ǰ��Mesh
	auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
	auto animator = GetGameObject()->GetComponent<Animator>();
	if (meshFilter == nullptr || animator == nullptr)
	{
		return;
	}

	if (m_isDirty)
	{
		CreateBoneBuffer();
		m_isDirty = false;
	}

	// get mesh
	auto model = meshFilter->GetMesh();

	// get bone data
	std::vector<int> boneHierarchy;
	std::vector<Matrix> boneOffsets;
	std::vector<Matrix> defaultTransform;
	model->GetBoneHierarchy(boneHierarchy);
	model->GetBoneOffsets(boneOffsets);
	model->GetNodeOffsets(defaultTransform);

	// get curr frame time and clip
	auto timePos = animator->GetCurrentTimePos();
	auto animationClip = animator->GetCurrentClip();

	// reset anim default trans
	animationClip->boneAnimations.resize(defaultTransform.size());
	for (size_t i = 0; i < defaultTransform.size(); i++)
		animationClip->boneAnimations[i].defaultTransform = defaultTransform[i];

	// calc bone trans to m_bone_arr
	CalcFinalTransform(timePos, animationClip, boneHierarchy, boneOffsets);

	// updat cbuffer
	m_bone_constant_buffer->UpdateWithReset(&m_bone_arr);
}

/**
 * \brief ��������ڵ�ǰ֡�����ձ任
 * \param timePos ��ǰʱ��ƫ��
 * \param clip ����Ƭ��
 * \param boneHierarchy �����㼶����
 * \param boneOffsets T-Pose ����, ���ڽ�����ת���������ռ�
 * \param finalTransforms �����������ձ任����
 */
void SkinnedMeshRenderer::CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets)
{
	uint32_t numBones = boneOffsets.size();
	std::vector<Matrix> toParentTransforms(numBones);

	clip->Interpolate(timePos, toParentTransforms);

	std::vector<Matrix> toRootTransforms(numBones);
	toRootTransforms[0] = toParentTransforms[0];

	// �������й�����root�ı任����
	for (uint32_t i = 1; i < numBones; i++) {
		int parentIndex = boneHierarchy[i];
		toRootTransforms[i] = toRootTransforms[parentIndex] * toParentTransforms[i];
	}

	// 
	for (uint32_t i = 0; i < numBones; i++) {
		m_bone_arr.boneArr[i] = toRootTransforms[i] * boneOffsets[i];
	}

}

void LitchiRuntime::SkinnedMeshRenderer::CreateBoneBuffer()
{
	m_bone_constant_buffer = std::make_shared<RHI_ConstantBuffer>();
	m_bone_constant_buffer->Create<Cb_Bone_Arr>(1);

	auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
	auto animator = GetGameObject()->GetComponent<Animator>();

	auto mesh = meshFilter->GetMesh();
	auto& bones = mesh->GetBones();
	int boneCount = bones.size();
	if(boneCount > MaxBoneCount)
	{
		DEBUG_LOG_ERROR("BoneCount Limit! boneCount:{}", boneCount);
		return;
	}

	m_bone_arr.boneCount = bones.size();

}
