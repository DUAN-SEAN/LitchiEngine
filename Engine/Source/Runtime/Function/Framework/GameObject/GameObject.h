
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <functional>
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"

namespace LitchiRuntime
{
	class Scene;
	class GameObject :public ScriptObject {
	public:
		GameObject() {}
		GameObject(const std::string& name, int64_t& id, bool& isPlaying);

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
		std::list<GameObject*> GetChildren();

		virtual void PostResourceLoaded() override;

	public:

		/**
		* Defines if the actor is sleeping or not.
		* A sleeping actor will not trigger methods suchs as OnEnable, OnDisable and OnDestroyed
		* @param p_sleeping
		*/
		void SetSleeping(bool p_sleeping);

		/**
		* Called when the scene start or when the actor gets enabled for the first time during play mode
		* This method will always be called in an ordered triple:
		* - OnAwake()
		* - OnEnable()
		* - OnStart()
		*/
		void OnAwake();

		/**
		* Called when the actor gets enabled (SetActive set to true) or at scene start if the actor is hierarchically active.
		* This method can be called in an ordered triple at scene start:
		* - OnAwake()
		* - OnEnable()
		* - OnStart()
		* Or can be called solo if the actor hierarchical active state changed to true and the actor already gets awaked
		* Conditions:
		* - Play mode only
		*/
		void OnEnable();

		/**
		* Called when the scene start or when the actor gets enabled for the first time during play mode
		* This method will always be called in an ordered triple:
		* - OnAwake()
		* - OnEnable()
		* - OnStart()
		*/
		void OnStart();

		/**
		* Called when the actor hierarchical active state changed to false or gets destroyed while being hierarchically active
		* Conditions:
		* - Play mode only
		*/
		void OnDisable();

		/**
		* Called when the actor gets destroyed if it has been awaked
		* Conditions:
		* - Play mode only
		*/
		void OnDestroy();


		/**
		* Called every frame
		*/
		void OnUpdate();

		/**
		* Called every physics frame
		*/
		void OnFixedUpdate();

		/**
		* Called every frame after OnUpdate
		*/
		void OnLateUpdate();

		/**
		* Called when the actor enter in collision with another physical object
		* @param p_otherObject
		*/
		void OnCollisionEnter(Collider* p_otherObject);

		/**
		* Called when the actor is in collision with another physical object
		* @param p_otherObject
		*/
		void OnCollisionStay(Collider* p_otherObject);

		/**
		* Called when the actor exit from collision with another physical object
		* @param p_otherObject
		*/
		void OnCollisionExit(Collider* p_otherObject);

		/**
		* Called when the actor enter in trigger with another physical object
		* @param p_otherObject
		*/
		void OnTriggerEnter(Collider* p_otherObject);

		/**
		* Called when the actor is in trigger with another physical object
		* @param p_otherObject
		*/
		void OnTriggerStay(Collider* p_otherObject);

		/**
		* Called when the actor exit from trigger with another physical object
		* @param p_otherObject
		*/
		void OnTriggerExit(Collider* p_otherObject);


	public:
		Scene* GetScene();
		void SetScene(Scene* scene);

		template <class T = Component>
		T* AddComponent() {
			T* component = new T();
			AttachComponent(component);
			component->PostResourceLoaded();

			if (m_isPlaying && GetActive())
			{
				component->OnAwake();
				component->OnEnable();
				component->OnStart();
			}
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
					delete component;
					return true;
				}
			}

			return false;
		}

		void OnEditorUpdate();

		int64_t m_id;
		int64_t m_parentId;
		std::vector<Component*> m_componentList;

		RTTR_ENABLE()
	private:
		void UnInitialize();

		void RecursiveActiveUpdate();
		void RecursiveWasActiveUpdate();
	private:

		/* Settings */
		Scene* m_scene = nullptr;
		bool m_active = true;
		bool m_isPlaying; 

		/* Internal settings */
		unsigned char m_layer;
		bool	m_destroyed = false;
		bool	m_sleeping = true; // is editor model sleep is true, so is default equal true
		bool	m_awaked = false;
		bool	m_started = false;
		bool	m_wasActive = false;
	};
}
