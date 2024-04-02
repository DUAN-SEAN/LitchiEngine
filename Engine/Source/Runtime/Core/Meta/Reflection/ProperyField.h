
#pragma once

#include <rttr/type>


namespace LitchiRuntime
{
	class Object;
	class Component;

	/**
	 * @brief Object property field
	 * record basic-type field location path in object
	 * example: materialRes.uniformList.1.position.x
	*/
	class PropertyField
	{
	public:

		/**
		 * @brief Construct from object, field path list
		 * @param root object handle 
		 * @param propertyNameList field path list
		*/
		PropertyField(Object* root, const std::vector<std::string>& propertyNameList);
		~PropertyField();

		/**
		 * @brief Set value from reflection warp value
		 * @param value reflection warp value
		 * @return return set success
		*/
		bool SetValue(rttr::variant value) const;

		/**
		 * @brief Get reflection warp value
		 * @return reflection warp value
		*/
		rttr::variant GetValue() const;

		/**
		 * @brief Get object
		 * @return object
		*/
		Object* GetObject() const { return m_root; }

	private:

		/**
		 * @brief root object
		*/
		Object* m_root;

		/**
		 * @brief basic-type field path list
		*/
		std::vector<std::string> m_propertyNameList;

	};
}
