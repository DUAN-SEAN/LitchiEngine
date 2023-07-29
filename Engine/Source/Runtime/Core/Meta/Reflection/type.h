
#pragma once

#include "rttr/registration"
#include "rttr/type"

#include "Runtime/AutoGen/Type/TypeRegister.h"

using namespace rttr;
namespace LitchiRuntime
{
	class TypeManager {
	public:

		static void Initialize(TypeManager* instance);

		/**
		 * \brief 获取类型句柄
		 * \tparam T
		 * \return
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
		 * \brief
		 * \tparam T 类型是否可序列化
		 * \return
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
		 * \brief 属性是否可序列化
		 * \tparam T
		 * \param propertyName
		 * \return
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
		static TypeManager* s_instance;

		std::vector<std::string> m_typeList;
	};
}
