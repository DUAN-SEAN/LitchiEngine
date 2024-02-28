
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{
	/**
	 * \brief 脚本组件基类
	 */
	class ScriptComponent :public Component
	{
	public:

		void SetClassName(std::string className)
		{
			m_className = className;
		}

		std::string GetClassName() const
		{
			return m_className;
		}

		void PostResourceLoaded() override;
		void PostResourceModify() override;

		void OnAwake() override;

		void OnUpdate() override;

	public:
		RTTR_ENABLE(Component);

	private:
		/**
		 * \brief 脚本类型
		 */
		std::string m_className;

		/**
		 * \brief 脚本数据, Json实现
		 */
		std::string m_data;
	};
}
