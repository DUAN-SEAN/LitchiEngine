
#ifndef UNTITLED_SERIALIZERMANAGER_H
#define UNTITLED_SERIALIZERMANAGER_H


#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

#include "rttr/registration"
#include "rttr/type"
#include "Runtime/Core/Meta/Reflection/type.h"

using namespace rttr;

class SerializerManager {
public:

	template <class T>
	rapidxml::xml_document<> Serialize(T* obj)
	{
		auto type = TypeManager::GetType<T>();

		// 序列化成员

		// 递归构造Node

		// 返回document

	}

	static void Initialize(SerializerManager* instance);

private:

	static SerializerManager* instance_;
};

#endif //UNTITLED_TYPE_H