#include "ScriptRegister.h"

#include "ScriptEngine.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

namespace LitchiRuntime
{

	void ScriptRegister::RegisterFunctions()
	{
		// ע�᷽��
	}

	void ScriptRegister::RegisterComponents()
	{
		// C++��GameObject��Components��C#���ӳ��

		// ��rttr�ķ�����Ϣ���õ�����

		std::string managedTypename = fmt::format("LitchiEngine.{}", "Transform");
		MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
	}


}

