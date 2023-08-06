#include "ScriptComponent.h"

#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"

namespace LitchiRuntime
{

	void LitchiRuntime::ScriptComponent::PostResourceLoaded()
	{
		// 通知ScriptEngine 创建托管脚本
		auto& className = m_className;

		// 通过class名创建脚本组件
		uint64_t unmanagedId = ScriptEngine::CreateScriptComponent(GetGameObject()->GetUnmanagedId(),className);
		if(unmanagedId == 0)
		{
			DEBUG_LOG_ERROR("CreateScriptComponent Fail className:{}", className);
		}else
		{
			m_unmanagedId = unmanagedId;
		}

	}

	void LitchiRuntime::ScriptComponent::PostResourceModify()
	{
		PostResourceLoaded();
	}

	void LitchiRuntime::ScriptComponent::Awake()
	{
		// 通知脚本引擎 执行回调Awake
		auto& unmanagedId = m_unmanagedId;
	}

	void LitchiRuntime::ScriptComponent::Update()
	{
		// 通知脚本引擎 执行回调Update
		auto& unmanagedId = m_unmanagedId;
	}
}
