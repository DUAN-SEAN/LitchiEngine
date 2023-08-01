#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"

#include <string>

namespace LitchiRuntime
{

	void ScriptEngine::Init()
	{
		InitMono();
	}

	void ScriptEngine::Shutdown()
	{
	}

	void ScriptEngine::InitMono()
	{
		// 设置mono所需的运行时程序集目录, 如果不设置将使用默认的位置
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("LitchiMonoRuntime");



		std::string scriptCoreDllPath = "../../ScriptCore/Debug/LitchiScriptCore.dll";
	}

	void ScriptEngine::ShutdownMono()
	{
	}

}
