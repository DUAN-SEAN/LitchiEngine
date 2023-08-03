#include "ScriptRegister.h"

#include "ScriptEngine.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

namespace LitchiRuntime
{

	void ScriptRegister::RegisterFunctions()
	{
		// 注册方法
	}

	void ScriptRegister::RegisterComponents()
	{
		// C++的GameObject和Components在C#层的映射

		// 从rttr的反射信息中拿到类型

		std::string managedTypename = fmt::format("LitchiEngine.{}", "Transform");
		MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
	}


}

