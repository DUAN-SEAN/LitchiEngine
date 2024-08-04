
#include "Runtime/Core/pch.h"
#include "Runtime/Core/Time/time.h"
#include "animator.h"

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Rendering/Animation.h"
#include "Runtime/Resource/ModelManager.h"

using namespace LitchiRuntime;


LitchiRuntime::Animator::Animator()
{
	m_animationClipInfoArr.push_back({ "Test","TestClipPath","TestSelectName" });
	m_animationClipInfoArr.push_back({ "Test2","TestClipPath2","TestSelectName2" });
}

LitchiRuntime::Animator::~Animator()
{
}

void LitchiRuntime::Animator::OnAwake()
{
}

void Animator::OnUpdate()
{
	m_timePos += Time::GetDeltaTime();

	// 更新Clip中的信息
	if(m_currentClip!=nullptr && m_timePos > m_currentClip->GetClipEndTime())
	{
		m_timePos = 0.0f;
	}
}

bool Animator::Play(std::string clipName)
{
	if(m_animationClipMap.find(clipName) == m_animationClipMap.end())
	{
		return false;
	}

	m_clipName = clipName;
	m_currentClip = &m_animationClipMap[clipName];
	m_timePos = 0.0f;

	return true;
}

AnimationClip* LitchiRuntime::Animator::GetCurrentClip() const
{
	return m_currentClip;
}

void LitchiRuntime::Animator::SetAnimationClipMap(std::unordered_map<std::string, AnimationClip>& clipMap)
{
	m_animationClipMap = clipMap;
}

void LitchiRuntime::Animator::PostResourceLoaded()
{
	PostResourceModify();
}

void LitchiRuntime::Animator::PostResourceModify()
{
	/*if(!m_animationClipInfoArr.empty())
	{
		for (const auto & animator_clip_info : m_animationClipInfoArr)
		{
			auto& path = animator_clip_info.m_clipPath;
			auto model = ApplicationBase::Instance()->modelManager->LoadResource(path);
			auto& animationClipMap  =model->GetAnimationClipMap();
			auto& animationClip = animationClipMap.at(animator_clip_info.m_selectClipResName);
		}
	}*/
}


