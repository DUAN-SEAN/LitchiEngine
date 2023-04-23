
#ifndef UNTITLED_TYPEMANAGER_H
#define UNTITLED_TYPEMANAGER_H

#include "rttr/registration"
#include "rttr/type"

#include "Runtime/AutoGen/Type/type_register.h"

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
		static TypeManager* instance_;

		std::vector<std::string> type_list_;
	};
}

#endif //UNTITLED_TYPEMANAGER_H