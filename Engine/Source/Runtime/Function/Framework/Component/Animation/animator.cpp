
#include "Runtime/Core/pch.h"
#include "Runtime/Core/Time/time.h"
#include "animator.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Rendering/Animation.h"

using namespace LitchiRuntime;


LitchiRuntime::Animator::Animator()
{
	m_animationNameArr.push_back({ "Test","TestClipPath","TestSelectName" });
	m_animationNameArr.push_back({ "Test2","TestClipPath2","TestSelectName2" });
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
	// todo 实例化时可以做一些事情
	// get default clip map

	/*if (m_animationClipMap.empty())
	{
		auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
		auto mesh = meshFilter->GetMesh();
		std::unordered_map<std::string, AnimationClip> animations;
		mesh->GetAnimations(animations);
		auto firstClipName = animations.begin()->first;
		SetAnimationClipMap(animations);
		Play(firstClipName);
	}*/
}

void LitchiRuntime::Animator::PostResourceModify()
{
}


