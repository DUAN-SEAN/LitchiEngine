

#include "type.h"

#include "Runtime/AutoGen/Type/type_register.h"
#include "Runtime/Core/Log/debug.h"
//#include "Runtime/AutoGen/Type/type_register_componet.h"
//#include "Runtime/AutoGen/Type/type_register_camera.h"
//#include "Runtime/AutoGen/Type/type_register_meshRenderer.h"
//#include "Runtime/AutoGen/Type/type_register_transform.h"

using namespace rttr;

void TypeManager::Initiliaze(TypeManager* instance)
{

	//获取类名
	auto typeList= type::get_types();

	for (rttr::type t : typeList)
	{
		if(t.is_class())
		{
			DEBUG_LOG_INFO(t.get_name().to_string());
			instance->type_list_.push_back(t.get_name().to_string());

			/*std::string typeName = t.get_name().to_string();
			instance->type_map_[typeName] = t;*/
		}
	}

	auto vec3Type = instance->GetType<glm::vec3>();
	auto methodList = vec3Type.get_methods();
	DEBUG_LOG_INFO(vec3Type.get_name().to_string());
	for (auto method :methodList)
	{
		DEBUG_LOG_INFO(method.get_name().to_string());
	}
	auto quatType = instance->GetType<glm::quat>();
}
