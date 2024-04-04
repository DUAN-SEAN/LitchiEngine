
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{
	/**
	 * @brief Script Component
	 * @note The ScriptComponent class is the base class from which every LitchiEngine script derives
	*/
	class ScriptComponent :public Component
	{
	public:

		/**
		 * @brief Set class name
		 * @param className 
		*/
		void SetClassName(std::string className)
		{
			m_className = className;
		}

		/**
		 * @brief Get class name
		 * @return 
		*/
		std::string GetClassName() const
		{
			return m_className;
		}

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		void PostResourceLoaded() override;

	public:

		/**
		 * @brief Called when the scene start right before OnStart
		 * It allows you to apply prioritized game logic on scene start
		*/
		void OnAwake() override;

		/**
		 * @brief Called when the components gets enabled (owner SetActive set to true) and after OnAwake() on scene starts
		*/
		void OnUpdate() override;

	private:

		/**
		 * @brief C# Script class name
		*/
		std::string m_className;

		/**
		 * @brief Script data, serialize to json
		*/
		std::string m_data;

		RTTR_ENABLE(Component);
	};
}
