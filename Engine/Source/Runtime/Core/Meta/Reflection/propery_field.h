
#pragma once

#include <rttr/type>


namespace LitchiRuntime
{
	class Object;
	class Component;
	class PropertyField
	{
	public:
		PropertyField(Object* root, const std::vector<std::string>& propertyNameList);
		~PropertyField();

		/**
		 * \brief 设置值
		 * \return
		 */
		bool SetValue(rttr::variant value) const;

		rttr::variant GetValue() const;

	private:
		Object* m_root;
		std::vector<std::string> m_propertyNameList;

	};
}
