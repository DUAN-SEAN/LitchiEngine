
#ifndef UNTITLED_TYPE_H
#define UNTITLED_TYPE_H

#include "rttr/registration"
#include "rttr/type"

using namespace rttr;

class TypeManager {
public:

	static void Initiliaze(TypeManager* instance);

	/**
	 * \brief 获取类型句柄
	 * \tparam T 
	 * \return 
	 */
	template <class T>
	rttr::type GetType()
	{
		//获取类名
		type t = type::get<T>();
		std::string component_type_name = t.get_name().to_string();
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
		return t.get_property(propertyName).get_metadata("Serializable").to_bool();
	}

private:
    static TypeManager* instance_;
	
	std::vector<std::string> type_list_;
};

#endif //UNTITLED_TYPE_H