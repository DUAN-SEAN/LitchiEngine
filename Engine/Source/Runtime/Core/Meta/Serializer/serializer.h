#pragma once

#include "rttr/registration"

using namespace rttr;
namespace LitchiRuntime
{

	class SerializerManager {
	public:

		static std::string SerializeToJson(rttr::instance obj);

		static bool DeserializeFromJson(const std::string& json, rttr::instance obj);
	private:
	};
}