
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering/Animation.h"


namespace LitchiRuntime
{
	struct AnimationClipInfo
	{
		std::string m_clipName;
		std::string m_clipPath;
		std::string m_selectClipResName;
		float m_startTime;
		float m_endTime;
		bool m_isLoop;
	};

	struct AnimationControlInfo
	{
		// clip 
	};

	/**
	 * @brief Simple Animator Component
	 * todo: This Component need add more function
	*/
	class Animator :public Component
	{
	public:

		Animator();
		~Animator();

		void OnAwake() override;
		void OnUpdate() override;

		void PostResourceLoaded() override;
		void PostResourceModify() override;

		/**
		 * @brief Play clip by clip name
		 * @param clipName 
		 * @return 
		*/
		bool Play(std::string clipName);

		/**
		 * \brief 获取当前播放的动画
		 * \return 返回Clip
		 */

		/**
		 * @brief Get current playing AnimationClip
		 * @return 
		*/
		AnimationClip& GetCurrentClip();

		/**
		 * @brief Get current playing AnimationClip name
		 * @return 
		*/
		std::string GetCurrentClipName()
		{
			return m_clipName;
		}

		/**
		 * @brief Get current Animation time position
		 * @return 
		*/
		float GetCurrentTimePos()
		{
			return m_timePos;
		}

		/**
		 * @brief Set AnimationClips
		 * @param clipMap clip name to AnimationClip dictionary
		*/
		void SetAnimationClipMap(std::unordered_map<std::string, AnimationClip>& clipMap);

		std::vector<AnimationClipInfo> m_animationClipInfoArr;

	private:

		/* temp fields */
		float m_timePos = 0.0f;
		std::string m_clipName = "";
		AnimationClip m_emptyAnimationClip{};
		std::unordered_map<std::string, AnimationClip> m_animationClipMap{};


		RTTR_ENABLE(Component)
	};

}
