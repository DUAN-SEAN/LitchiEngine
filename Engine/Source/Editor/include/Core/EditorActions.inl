
#pragma once

#include "EditorActions.h"

namespace LitchiEditor
{
	template<typename T>
	inline LitchiRuntime::GameObject* EditorActions::CreateMonoComponentActor(bool p_focusOnCreation, LitchiRuntime::GameObject* p_parent)
	{
		auto instance = CreateEmptyActor(false, p_parent);

		T* component = instance->AddComponent<T>();

        instance->SetName("Test");

		if (p_focusOnCreation)
			SelectActor(instance);

		return instance;
	}

	template<typename T>
	inline LitchiRuntime::GameObject* EditorActions::CreateUIActor(bool p_focusOnCreation, LitchiRuntime::GameObject* p_parent)
	{
		auto instance = CreateEmptyActor(false, p_parent,"",true);

		T* component = instance->AddComponent<T>();

		instance->SetName("UI Element");

		if (p_focusOnCreation)
			SelectActor(instance);

		return instance;
	}
}