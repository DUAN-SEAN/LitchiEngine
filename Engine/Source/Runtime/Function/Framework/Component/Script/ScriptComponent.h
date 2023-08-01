
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{
	/**
	 * \brief �ű��������
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

	public:
		RTTR_ENABLE(Component);

	private:
		/**
		 * \brief �ű�����
		 */
		std::string m_className;

		/**
		 * \brief �ű�����, Jsonʵ��
		 */
		std::string m_data;
	};
}
