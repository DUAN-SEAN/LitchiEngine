﻿
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <functional>

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Script/ScriptComponent.h"

namespace LitchiRuntime
{
	class Scene;
	class GameObject :public ScriptObject {
	public:
		GameObject() {}
		GameObject(const std::string& name, int64_t& id, bool& isPlaying,Scene* scene);

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

		/**
		 * \brief call in editor not play mode
		 */
		void OnEditorUpdate();

	public: 
		Scene* GetScene();
		void SetScene(Scene* scene);

		template <class T = Component>
		T* AddComponent();

		template <class T = Component>
		void AttachComponent(T* component);

		template <class T = Component>
		T* GetComponent() const;

		template <class T = Component>
		T* GetComponent(const uint64_t unmanagedId);

		std::vector<Component*>& GetComponents() { return m_componentList; }

		void ForeachComponent(std::function<void(Component*)> func);
		bool RemoveComponent(Component* component);

		int64_t m_id;
		int64_t m_parentId;
		std::vector<Component*> m_componentList;

		RTTR_ENABLE()
	public:
		/* Some events that are triggered when an action occur on the actor instance */
		Event<Component*>	ComponentAddedEvent;
		Event<Component*>	ComponentRemovedEvent;
		Event<ScriptComponent*>	BehaviourAddedEvent;
		Event<ScriptComponent*>	BehaviourRemovedEvent;

		/* Some events that are triggered when an action occur on any actor */
		static Event<GameObject*>				DestroyedEvent;
		static Event<GameObject*>				CreatedEvent;
		static Event<GameObject*, GameObject*>		AttachEvent;
		static Event<GameObject*>				DettachEvent;
	private:
		void UnInitialize();

		void RecursiveActiveUpdate();
		void RecursiveWasActiveUpdate();
	private:

		/* Settings */
		Scene* m_scene = nullptr; // if null, not scene gameObject, maybe used to prefab
		bool m_active = true;
		bool m_isPlaying {false};

		/* Internal settings */
		unsigned char m_layer;
		bool	m_destroyed = false;
		bool	m_sleeping = true; // is editor model sleep is true, so is default equal true
		bool	m_awaked = false;
		bool	m_started = false;
		bool	m_wasActive = false;
	};
}

#include "GameObject.inl"
