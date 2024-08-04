
#include "Runtime/Core/pch.h"
#include "SkinnedMeshRenderer.h"
#include "MeshFilter.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RHI/RHI_ConstantBuffer.h"

using namespace LitchiRuntime;

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

void SkinnedMeshRenderer::OnAwake()
{

}

void SkinnedMeshRenderer::OnUpdate()
{
	MeshRenderer::OnUpdate();

	if (m_isFirstTick && m_material!= nullptr)
	{
		// todo do some thing
		m_isFirstTick = false;
	}

	if (m_isDirty)
	{
		CreateBoneBuffer();
		m_isDirty = false;
	}

	// 通过MeshFilter获取当前的Mesh
	auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
	auto animator = FindAnimatorInHierarchy();
	if (meshFilter == nullptr || animator == nullptr)
	{
		return;
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

	// calc bone trans to m_boneArr
	CalcFinalTransform(timePos, animationClip, boneHierarchy, boneOffsets);

	//uint32_t numBones = boneOffsets.size();
	//std::vector<Matrix> toRootTransforms(numBones);
	//toRootTransforms[0] = animationClip->boneAnimations[0].defaultTransform;
	//// 计算所有骨骼到root的变换矩阵
	//for (uint32_t i = 1; i < numBones; i++) {
	//	int parentIndex = boneHierarchy[i];
	//	toRootTransforms[i] = toRootTransforms[parentIndex] * animationClip->boneAnimations[i].defaultTransform;
	//	m_boneArr.boneArr[i] = toRootTransforms[i] * boneOffsets[i];
	//}
	//
	// updat cbuffer
	m_boneConstantBuffer->UpdateWithReset(&m_boneArr);
}

void SkinnedMeshRenderer::PostResourceLoaded()
{
	// 通过MeshFilter获取当前的Mesh
	auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
	if (meshFilter == nullptr)
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
	std::vector<Matrix> defaultTransform;
	model->GetBoneHierarchy(boneHierarchy);
	model->GetNodeOffsets(defaultTransform);

	CalcDefaultFinalTransform(boneHierarchy, defaultTransform);
	// updat cbuffer
	m_boneConstantBuffer->UpdateWithReset(&m_boneArr);

	MeshRenderer::PostResourceLoaded();
}

void LitchiRuntime::SkinnedMeshRenderer::SetDefaultMaterial()
{
	Material* material = ApplicationBase::Instance()->materialManager->LoadResource(":Materials/StandardSkinn4Phong.mat");
	SetMaterial(material);
	m_useDefaultMaterial = true;
}

void SkinnedMeshRenderer::PostResourceModify()
{
	MeshRenderer::PostResourceModify();
}

void SkinnedMeshRenderer::CreateDefaultBoneBuffer()
{
}

void SkinnedMeshRenderer::CalcDefaultFinalTransform(std::vector<int>& boneHierarchy, std::vector<Matrix>& nodelDefaultTransforms)
{
	uint32_t numBones = nodelDefaultTransforms.size();
	// 
	for (uint32_t i = 0; i < numBones; i++) {
		m_boneArr.boneArr[i] = nodelDefaultTransforms[i];
	}
}

/**
 * \brief 计算骨骼在当前帧的最终变换
 * \param timePos 当前时间偏移
 * \param clip 动画片段
 * \param boneHierarchy 骨骼层级索引
 * \param boneOffsets T-Pose 矩阵, 用于将顶点转换到骨骼空间
 * \param finalTransforms 输出顶点的最终变换矩阵
 */
void SkinnedMeshRenderer::CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets)
{
	uint32_t numBones = boneOffsets.size();
	std::vector<Matrix> toParentTransforms(numBones);

	clip->Interpolate(timePos, toParentTransforms);

	std::vector<Matrix> toRootTransforms(numBones);
	toRootTransforms[0] = toParentTransforms[0];

	// 计算所有骨骼到root的变换矩阵
	for (uint32_t i = 1; i < numBones; i++) {
		int parentIndex = boneHierarchy[i];
		toRootTransforms[i] = toParentTransforms[i] *toRootTransforms[parentIndex];
	}

	// 
	for (uint32_t i = 0; i < numBones; i++) {
		m_boneArr.boneArr[i] = boneOffsets[i]* toRootTransforms[i];
	}

}

Animator* LitchiRuntime::SkinnedMeshRenderer::FindAnimatorInHierarchy()
{
	GameObject* currGo = GetGameObject();
	while (true)
	{
		if(currGo == nullptr)
		{
			return nullptr;
		}

		auto* animator = currGo->GetComponent<Animator>();
		if(animator != nullptr)
		{
			return animator;
		}

		if(!currGo->HasParent())
		{
			return nullptr;
		}

		currGo = currGo->GetParent();
	}
}

void LitchiRuntime::SkinnedMeshRenderer::CreateBoneBuffer()
{
	m_boneConstantBuffer = std::make_shared<RHI_ConstantBuffer>();
	m_boneConstantBuffer->Create<Cb_Bone_Arr>(1);

	auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();

	auto mesh = meshFilter->GetMesh();
	auto& bones = mesh->GetBones();
	int boneCount = bones.size();
	if(boneCount > MaxBoneCount)
	{
		DEBUG_LOG_ERROR("BoneCount Limit! boneCount:{}", boneCount);
		return;
	}

	// m_boneArr.boneCount = bones.size();

}
