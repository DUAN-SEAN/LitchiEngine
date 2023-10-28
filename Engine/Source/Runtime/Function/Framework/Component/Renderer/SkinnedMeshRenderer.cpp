
#include "SkinnedMeshRenderer.h"
#include "MeshFilter.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

using namespace LitchiRuntime;

LitchiRuntime::SkinnedMeshRenderer::SkinnedMeshRenderer()
{
}

LitchiRuntime::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

void SkinnedMeshRenderer::Update()
{
	// TODO
	//// ͨ��MeshFilter��ȡ��ǰ��Mesh
	//auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
	//auto animator = GetGameObject()->GetComponent<Animator>();
	//if (meshFilter == nullptr || animator == nullptr)
	//{
	//	return;
	//}

	//// ��ȡģ��
	//auto model = meshFilter->GetModel();

	//// ��ȡģ���ϵĹ�����Ϣ
	//std::vector<int> boneHierarchy;
	//std::vector<Matrix> boneOffsets;
	//std::vector<Matrix> defaultTransform;
	//model->GetBoneHierarchy(boneHierarchy);
	//model->GetBoneOffsets(boneOffsets);
	//model->GetNodeOffsets(defaultTransform);


	//// ��ȡ��ǰ֡��Ϣ
	//auto timePos = animator->GetCurrentTimePos();
	//auto animationClip = animator->GetCurrentClip();

	//// ����node��Ĭ�ϱ任
	//animationClip->boneAnimations.resize(defaultTransform.size());
	//for (size_t i = 0; i < defaultTransform.size(); i++)
	//	animationClip->boneAnimations[i].defaultTransform = defaultTransform[i];

	//CalcFinalTransform(timePos, animationClip, boneHierarchy, boneOffsets, m_finalTransformCacheArr);
}

//void SkinnedMeshRenderer::Render(RenderCamera* renderCamera, Matrix const* lightVPMat, Framebuffer4Depth* shadowMapFBO)
//{
//	// �ύFinalTransform��GPU
//	GetMaterial()->GetShader()->Bind();
//	GetMaterial()->GetShader()->SetUniformMat4("ubo_boneFinalMatrixArr[0]", *m_finalTransformCacheArr.data(), m_finalTransformCacheArr.size());
//	GetMaterial()->GetShader()->Unbind();
//
//	MeshRenderer::Render(renderCamera, lightVPMat, shadowMapFBO);
//}
//
//void SkinnedMeshRenderer::RenderShadowMap()
//{
//	MeshRenderer::RenderShadowMap();
//}

/**
 * \brief ��������ڵ�ǰ֡�����ձ任
 * \param timePos ��ǰʱ��ƫ��
 * \param clip ����Ƭ��
 * \param boneHierarchy �����㼶����
 * \param boneOffsets T-Pose ����, ���ڽ�����ת���������ռ�
 * \param finalTransforms �����������ձ任����
 */
void SkinnedMeshRenderer::CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets, std::vector<Matrix>& finalTransforms)
{
	uint32_t numBones = boneOffsets.size();
	finalTransforms.resize(numBones);
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
		finalTransforms[i] = toRootTransforms[i] * boneOffsets[i];
	}

}
