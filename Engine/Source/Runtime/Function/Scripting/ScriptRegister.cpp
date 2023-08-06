#include "ScriptRegister.h"

#include "ScriptEngine.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

namespace LitchiRuntime
{

#if 1 外部绑定代码

	#define LitchiEngine_ADD_INTERNAL_CALL(Name) mono_add_internal_call("LitchiEngine.InternalCalls::" #Name, Name)

	static MonoObject* GetScriptInstance(uint64_t unmanagedId)
	{
		return ScriptEngine::GetManagedInstance(unmanagedId);
	}

	void ScriptRegister::RegisterFunctions()
	{
		// 注册方法
		LitchiEngine_ADD_INTERNAL_CALL(GetScriptInstance);
	}

#endif

	void ScriptRegister::RegisterComponents()
	{
		// C++的GameObject和Components在C#层的映射

		// 从rttr的反射信息中拿到类型

		std::string managedTypename = fmt::format("LitchiEngine.{}", "Transform");
		MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
	}


}

