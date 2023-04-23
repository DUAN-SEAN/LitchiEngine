
#ifndef UNTITLED_SERIALIZERMANAGER_H
#define UNTITLED_SERIALIZERMANAGER_H


#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

#include "rttr/registration"
#include "rttr/type"
#include "Runtime/Core/Meta/Reflection/type.h"

using namespace rttr;
namespace LitchiRuntime
{

	class SerializerManager {
	public:

		std::string SerializeToJson(rttr::instance obj);

		bool DeserializeFromJson(const std::string& json, rttr::instance obj);

		static void Initialize(SerializerManager* instance);

	private:
	};
}
#endif //UNTITLED_TYPE_H