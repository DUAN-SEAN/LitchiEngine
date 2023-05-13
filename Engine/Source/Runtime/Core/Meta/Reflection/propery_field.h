
#pragma once

#include <rttr/type>

namespace LitchiRuntime
{
	class PropertyField
	{
	public:
		PropertyField(rttr::type type,std::vector<std::string> propertyNameList);
		~PropertyField();

		/**
		 * \brief …Ë÷√÷µ
		 * \return
		 */
		bool SetValue(rttr::instance instance, rttr::variant value);

	private:
		rttr::type m_type;
		std::vector<std::string> m_propertyNameList;

	};
}
