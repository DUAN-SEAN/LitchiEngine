
#ifndef UNTITLED_TYPE_H
#define UNTITLED_TYPE_H

#include "rttr/registration"
#include "rttr/type"

using namespace rttr;

class TypeManager {
public:

	template <class T>
	rttr::type GetType()
	{
		//获取类名
		type t = type::get<T>();
		std::string component_type_name = t.get_name().to_string();
		return t;	
	}

    static void Initiliaze(TypeManager* instance);

private:
    static TypeManager* instance_;


    std::unordered_map<std::string, rttr::type> type_map_;

	std::vector<std::string> type_list_;

};

#endif //UNTITLED_TYPE_H