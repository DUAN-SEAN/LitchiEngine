
#pragma once

#include "EditorActions.h"

namespace LitchiEditor
{
	template<typename T>
	inline LitchiRuntime::GameObject* EditorActions::CreateMonoComponentActor(bool p_focusOnCreation, LitchiRuntime::GameObject* p_parent)
	{
		auto& instance = CreateEmptyActor(false, p_parent);

		T* component = instance.AddComponent<T>();

        instance->set_name("Test");

		if (p_focusOnCreation)
			SelectActor(instance);

		return instance;
	}
}