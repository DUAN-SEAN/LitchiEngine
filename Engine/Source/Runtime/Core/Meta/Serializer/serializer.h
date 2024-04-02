#pragma once

#include "rttr/registration"

using namespace rttr;
namespace LitchiRuntime
{
	/**
	 * @brief Serializer 
	*/
	class Serializer {
	public:

		/**
		 * @brief Serialize reflection object to json text
		 * @param obj Reflection object
		 * @return 
		*/
		static std::string SerializeToJson(rttr::instance obj);

		/**
		 * @brief Deserialize json text to reflection object
		 * @param json Json text
		 * @param obj Reflection object
		 * @return 
		*/
		static bool DeserializeFromJson(const std::string& json, rttr::instance obj);
	};
}