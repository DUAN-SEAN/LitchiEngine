
#ifndef UNTITLED_SERIALIZERMANAGER_H
#define UNTITLED_SERIALIZERMANAGER_H

#include "rttr/registration"

using namespace rttr;
namespace LitchiRuntime
{

	class SerializerManager {
	public:

		static std::string SerializeToJson(rttr::instance obj);

		static bool DeserializeFromJson(const std::string& json, rttr::instance obj);

		static void Initialize(SerializerManager* instance);

	private:
	};
}
#endif //UNTITLED_TYPE_H