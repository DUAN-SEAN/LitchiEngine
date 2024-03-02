
#pragma once

#include "GameObject.h"

namespace LitchiRuntime
{
	template <class T>
	inline T* GameObject::AddComponent() {
		T* component = new T();
		AttachComponent(component);
		component->PostResourceLoaded();

		ComponentAddedEvent.Invoke(component);

		if (m_isPlaying && GetActive())
		{
			component->OnAwake();
			component->OnEnable();
			component->OnStart();
		}
		return dynamic_cast<T*>(component);
	}

	template <class T>
	inline void GameObject::AttachComponent(T* component)
	{
		component->SetGameObject(this);
		//获取类名
		type t = type::get<T>();
		std::string component_type_name = t.get_name().to_string();
		component->SetObjectName(component_type_name);

		m_componentList.push_back(component);

	}

	template <class T>
	inline T* GameObject::GetComponent() const {
		//获取类名
		type t = type::get<T>();
		std::string component_type_name = t.get_name().to_string();
		std::vector<Component*> component_vec;

		for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
		{
			if ((*iter)->get_type().get_name() == component_type_name)
			{
				return dynamic_cast<T*>(*iter);
			}
		}

		// Find Drived Class
		auto derived_classes = t.get_derived_classes();
		for (auto derived_class : derived_classes) {
			std::string derived_class_type_name = derived_class.get_name().to_string();

			for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
			{
				if ((*iter)->get_type().get_name() == derived_class_type_name)
				{
					return dynamic_cast<T*>(*iter);
				}
			}
		}

		return nullptr;
	}

	template <class T>
	inline T* GameObject::GetComponent(const uint64_t unmanagedId) {
		//获取类名
		type t = type::get<T>();
		std::string component_type_name = t.get_name().to_string();
		std::vector<Component*> component_vec;

		for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
		{
			if ((*iter)->get_type().get_name() == component_type_name && (*iter)->GetUnmanagedId() == unmanagedId)
			{
				return dynamic_cast<T*>(*iter);
			}
		}

		// Find Drived Class
		auto derived_classes = t.get_derived_classes();
		for (auto derived_class : derived_classes) {
			std::string derived_class_type_name = derived_class.get_name().to_string();

			for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
			{
				if ((*iter)->get_type().get_name() == derived_class_type_name && (*iter)->GetUnmanagedId() == unmanagedId)
				{
					return dynamic_cast<T*>(*iter);
				}
			}
		}


		return nullptr;
	}
}
