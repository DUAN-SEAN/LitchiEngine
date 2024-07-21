
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering/Animation.h"


namespace LitchiRuntime
{
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
		AnimationClip* GetCurrentClip() const;

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

		std::vector<std::string> m_animationNameArr;

	private:

		/* temp fields */
		float m_timePos = 0.0f;
		std::string m_clipName = "";
		std::unordered_map<std::string, AnimationClip> m_animationClipMap{};
		AnimationClip* m_currentClip = nullptr;


		RTTR_ENABLE(Component)
	};

}
