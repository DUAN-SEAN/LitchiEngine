
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <functional>
#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{
	class Scene;
	class GameObject :public ScriptObject{
	public:
		GameObject() {}
		GameObject(std::string name,int64_t id);

		~GameObject();

		void Initialize();

		std::string GetName() { return GetObjectName(); }
		void SetName(std::string name) { SetObjectName(name); }

		unsigned char GetLayer() { return m_layer; }
		void SetLayer(unsigned char layer) { m_layer = layer; }

		bool GetActive() { return m_active; }
		void SetActive(bool active);
		
		bool SetParent(GameObject* parent);
		bool HasParent();
		GameObject* GetParent();
		std::list<GameObject*>& GetChildren();

		virtual void PostResourceLoaded() override;

	public:
		Scene* GetScene();
		void SetScene(Scene* scene);
		
		template <class T = Component>
		T* AddComponent() {
			T* component = new T();
			AttachComponent(component);
			component->Awake();
			return dynamic_cast<T*>(component);
		}
		
		template <class T = Component>
		void AttachComponent(T* component)
		{
			component->SetGameObject(this);
			//获取类名
			type t = type::get<T>();
			std::string component_type_name = t.get_name().to_string();
			component->SetObjectName(component_type_name);

			m_componentList.push_back(component);
			
		}
		
		template <class T = Component>
		T* GetComponent() const {
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
			return nullptr;
		}
		
		template <class T = Component>
		T* GetComponent(const uint64_t unmanagedId) {
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
			return nullptr;
		}

		std::vector<Component*>& GetComponents() { return m_componentList; }
		
		void ForeachComponent(std::function<void(Component*)> func);
		bool RemoveComponent(Component* component)
		{
			//获取类名
			type t = component->get_type();
			std::string component_type_name = t.get_name().to_string();
			for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
			{
				if (*iter == component)
				{
					m_componentList.erase(iter);
					// todo: delete iter
					delete *iter;
					return true;
				}
			}

			return false;
		}

		int64_t m_id;
		int64_t m_parentId;
		std::vector<Component*> m_componentList;

		RTTR_ENABLE()
	private:
		void UnInitialize();

	private:
		// 将物体分不同的层，用于相机分层、物理碰撞分层等。
		unsigned char m_layer;
		// 场景
		Scene* m_scene;
		// 是否激活
		bool m_active = true;
	};
}
