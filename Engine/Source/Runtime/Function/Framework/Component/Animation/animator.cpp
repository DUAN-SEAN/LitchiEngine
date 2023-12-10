
#include "Runtime/Core/Time/time.h"
#include "animator.h"
#include "Runtime/Function/Renderer/Rendering/Animation.h"

using namespace LitchiRuntime;


LitchiRuntime::Animator::Animator()
{
}

LitchiRuntime::Animator::~Animator()
{
}

void LitchiRuntime::Animator::Awake()
{
	// todo 实例化时可以做一些事情
}

void Animator::Update()
{
	m_timePos += Time::delta_time();

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


