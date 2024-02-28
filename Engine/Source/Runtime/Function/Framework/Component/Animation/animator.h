
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering/Animation.h"


namespace LitchiRuntime
{
	class Animator :public Component
	{
	public:

		Animator();
		~Animator();
		void OnAwake() override;

		void OnUpdate() override;

		/**
		 * \brief 播放动画
		 * \param clipName 
		 */
		bool Play(std::string clipName);

		/**
		 * \brief 获取当前播放的动画
		 * \return 返回Clip
		 */
		AnimationClip* GetCurrentClip() const;

		std::string GetCurrentClipName()
		{
			return m_clipName;
		}

		float GetCurrentTimePos()
		{
			return m_timePos;
		}

		void SetAnimationClipMap(std::unordered_map<std::string, AnimationClip>& clipMap);

	public:

		void PostResourceLoaded() override;
		void PostResourceModify() override;

		RTTR_ENABLE(Component)

	private:

		float m_timePos = 0.0f;
		std::string m_clipName = "";


		std::unordered_map<std::string, AnimationClip> m_animationClipMap;

		AnimationClip* m_currentClip = nullptr;
	};

}
