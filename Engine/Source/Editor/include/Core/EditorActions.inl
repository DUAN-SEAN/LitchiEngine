///**
//* @project: Overload
//* @author: Overload Tech.
//* @licence: MIT
//*/
//
//#pragma once
//
//#include "OvEditor/Core/EditorActions.h"
//
//namespace OvEditor::Core
//{
//	template<typename T>
//	inline GameObject* EditorActions::CreateMonoComponentActor(bool p_focusOnCreation, GameObject* p_parent)
//	{
//		auto& instance = CreateEmptyActor(false, p_parent);
//
//		T& component = instance.AddComponent<T>();
//
//        instance.SetName(component.GetName());
//
//		if (p_focusOnCreation)
//			SelectActor(instance);
//
//		return instance;
//	}
//}