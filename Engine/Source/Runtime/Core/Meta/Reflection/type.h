
#pragma once

#include "rttr/registration"
#include "rttr/type"

#include "Runtime/AutoGen/Type/TypeRegister.h"

using namespace rttr;
namespace LitchiRuntime
{
	class TypeManager {
	public:

		/**
		 * @brief Get type handle
		 * @tparam T 
		 * @return 
		*/
		template <class T>
		rttr::type GetType()
		{
			// 获取类名
			type t = type::get<T>();
			if (!t.is_valid())
			{
				return rttr::type::get();
			}

			return t;
		}

		/**
		 * @brief Check type can serialize
		 * @tparam T 
		 * @return 
		*/
		template <class T>
		bool IsSerializable()
		{
			type t = type::get<T>();
			if (!t.is_valid())
			{
				return false;
			}

			return t.get_metadata("Serializable").to_bool();
		}

		/**
		 * @brief Check property can serialize
		 * @tparam T 
		 * @param propertyName property name
		 * @return 
		*/
		template <class T>
		bool IsSerializable(std::string propertyName)
		{
			type t = type::get<T>();
			if (!t.is_valid())
			{
				return false;
			}

			return t.get_property(propertyName).get_metadata("Serializable").to_bool();
		}

	private:

		/**
		 * @brief singleton
		*/
		static TypeManager* s_instance;

		/**
		 * @brief type group 
		*/
		std::vector<std::string> m_typeList;
	};
}
