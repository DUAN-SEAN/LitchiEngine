
#pragma once

#include <rttr/type>


namespace LitchiRuntime
{
	class Object;
	class Component;
	class PropertyField
	{
	public:
		PropertyField(Component* root,const std::vector<std::string>& propertyNameList);
		~PropertyField();

		/**
		 * \brief …Ë÷√÷µ
		 * \return
		 */
		bool SetValue(rttr::variant value) const;

	private:
		Component* m_root;
		std::vector<std::string> m_propertyNameList;

	};
}
