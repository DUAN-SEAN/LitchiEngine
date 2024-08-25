
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
	if(!m_clipName.empty())
	{
		auto& currClip = GetCurrentClip();
		if(m_timePos > currClip.GetClipEndTime())
		{
			m_timePos = 0.0f;
		}
	}
}

bool Animator::Play(std::string clipName)
{
	if(m_animationClipMap.find(clipName) == m_animationClipMap.end())
	{
		return false;
	}

	m_clipName = clipName;
	m_timePos = 0.0f;

	return true;
}

AnimationClip& LitchiRuntime::Animator::GetCurrentClip()
{
	if(!m_clipName.empty() && m_animationClipMap.size()>0)
	{
		if(m_animationClipMap.find(m_clipName)!= m_animationClipMap.end())
		{
			return m_animationClipMap.at(m_clipName);
		}
	}

	return m_emptyAnimationClip;
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
	if (!m_animationClipInfoArr.empty())
	{
		m_animationClipMap.clear();
		for (const auto& animation_clip_info : m_animationClipInfoArr)
		{
			auto& path = animation_clip_info.m_clipPath;
			const auto model = ApplicationBase::Instance()->modelManager->LoadResource(path);
			auto& animationClipMap = model->GetAnimationClipMap();

			// if selectResName = empty, try find first clip
			if (animation_clip_info.m_selectClipResName.empty() && animationClipMap.size() > 0)
			{
				auto& animationClip = animationClipMap.cbegin()->second;
				m_animationClipMap.emplace(animation_clip_info.m_clipName, animationClip);
			}
			else
			{
				if(animationClipMap.find((animation_clip_info.m_selectClipResName)) != animationClipMap.end())
				{
					auto& animationClip = animationClipMap.at(animation_clip_info.m_selectClipResName);
					m_animationClipMap.emplace(animation_clip_info.m_clipName, animationClip);
				}
			}
		}

		m_clipName = m_animationClipInfoArr.front().m_clipName;
	}


}


