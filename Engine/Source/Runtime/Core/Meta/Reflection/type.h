
#ifndef UNTITLED_TYPE_H
#define UNTITLED_TYPE_H

#include "rttr/registration"
#include "rttr/type"

using namespace rttr;

class TypeManager {
public:

	static void Initiliaze(TypeManager* instance);

	/**
	 * \brief ��ȡ���;��
	 * \tparam T 
	 * \return 
	 */
	template <class T>
	rttr::type GetType()
	{
		//��ȡ����
		type t = type::get<T>();
		std::string component_type_name = t.get_name().to_string();
		return t;	
	}

	/**
	 * \brief 
	 * \tparam T �����Ƿ�����л�
	 * \return 
	 */
	template <class T>
	bool IsSerializable()
	{
		type t = type::get<T>();
		return t.get_metadata("Serializable").to_bool();
	}

	/**
	 * \brief �����Ƿ�����л�
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