#include "ScriptComponent.h"

namespace LitchiRuntime
{

	void LitchiRuntime::ScriptComponent::PostResourceLoaded()
	{
		// 通知ScriptEngine 创建托管脚本
		auto& className = m_className;

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
