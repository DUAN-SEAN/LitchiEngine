
#include "skinned_mesh_renderer.h"

#include "mesh_filter.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

using namespace LitchiRuntime;

LitchiRuntime::SkinnedMeshRenderer::SkinnedMeshRenderer()
{
}

LitchiRuntime::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

void SkinnedMeshRenderer::Update()
{
	// 通过MeshFilter获取当前的Mesh
	auto meshFilter = game_object()->GetComponent<MeshFilter>();
	auto animator = game_object()->GetComponent<Animator>();
	if (meshFilter == nullptr || animator == nullptr)
	{
		return;
	}

	// 获取模型
	auto model = meshFilter->GetModel();

	// 获取模型上的骨骼信息
	std::vector<int> boneHierarchy;
	std::vector<glm::mat4> boneOffsets;
	std::vector<glm::mat4> defaultTransform;
	model->GetBoneHierarchy(boneHierarchy);
	model->GetBoneOffsets(boneOffsets);
	model->GetNodeOffsets(defaultTransform);


	// 获取当前帧信息
	auto timePos = animator->GetCurrentTimePos();
	auto animationClip = animator->GetCurrentClip();

	// 重置node的默认变换
	animationClip->boneAnimations.resize(defaultTransform.size());
	for (size_t i = 0; i < defaultTransform.size(); i++)
		animationClip->boneAnimations[i].defaultTransform = defaultTransform[i];

	CalcFinalTransform(timePos, animationClip, boneHierarchy, boneOffsets, m_finalTransformCacheArr);
}

void SkinnedMeshRenderer::Render(RenderCamera* renderCamera, glm::mat4 const* lightVPMat, Framebuffer4Depth* shadowMapFBO)
{
	// 提交FinalTransform到GPU
	material()->GetShader()->Bind();
	material()->GetShader()->SetUniformMat4("ubo_boneFinalMatrixArr[0]", *m_finalTransformCacheArr.data(), m_finalTransformCacheArr.size());
	material()->GetShader()->Unbind();

	MeshRenderer::Render(renderCamera, lightVPMat, shadowMapFBO);
}

void SkinnedMeshRenderer::RenderShadowMap()
{
	// 提交FinalTransform到GPU
	material()->GetShader()->Bind();
	material()->GetShader()->SetUniformMat4("ubo_boneFinalMatrixArr[0]", *m_finalTransformCacheArr.data(),m_finalTransformCacheArr.size());
	material()->GetShader()->Unbind();

	MeshRenderer::RenderShadowMap();
}

/**
 * \brief 计算骨骼在当前帧的最终变换
 * \param timePos 当前时间偏移
 * \param clip 动画片段
 * \param boneHierarchy 骨骼层级索引
 * \param boneOffsets T-Pose 矩阵, 用于将顶点转换到骨骼空间
 * \param finalTransforms 输出顶点的最终变换矩阵
 */
void SkinnedMeshRenderer::CalcFinalTransform(float timePos, AnimationClip* clip, std::vector<int>& boneHierarchy, std::vector<glm::mat4>& boneOffsets, std::vector<glm::mat4x4>& finalTransforms)
{
	uint32_t numBones = boneOffsets.size();
	finalTransforms.resize(numBones);
	std::vector<glm::mat4x4> toParentTransforms(numBones);

	clip->Interpolate(timePos, toParentTransforms);

	std::vector<glm::mat4x4> toRootTransforms(numBones);
	toRootTransforms[0] = toParentTransforms[0];

	// 计算所有骨骼到root的变换矩阵
	for (uint32_t i = 1; i < numBones; i++) {
		int parentIndex = boneHierarchy[i];
		toRootTransforms[i] = toRootTransforms[parentIndex] * toParentTransforms[i];
	}

	// 
	for (uint32_t i = 0; i < numBones; i++) {
		finalTransforms[i] = toRootTransforms[i] * boneOffsets[i];
	}

}
