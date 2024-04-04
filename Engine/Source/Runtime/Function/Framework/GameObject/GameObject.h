
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

	/**
	 * @brief GameObject, Attach to Scene or Prefab.
	 * Composite some components, add or remove component
	 * Drive component awake, update, destroy
	 */
	class GameObject :public ScriptObject {
	public:

		/**
		 * @brief Default Constructor, reflect may call this
		 */
		GameObject() {}

		/**
		 * @brief Construct from name, id, isPlaying and attached scene
		 * @param name GameObject name
		 * @param id Unique Id within the same scene, will store to serialize file  
		 * @param isPlaying Is playing mode, not editor mode
		 * @param scene attached scene
		 */
		GameObject(const std::string& name, int64_t& id, bool& isPlaying, Scene* scene);

		/**
		 * @brief Destructor
		 */
		~GameObject() override;

		/**
		 * @brief Set GameObject name
		 * @param name 
		*/
		void SetName(std::string name) { SetObjectName(name); }

		/**
		 * @brief Get GameObject name
		 * @return 
		*/
		std::string GetName() { return GetObjectName(); }

		/**
		 * @brief Get bind scene
		 * if gameObject belonged prefab, return null
		 * @return bind scene
		*/
		Scene* GetScene();

		/**
		 * @brief Set bind scene
		 * @param scene
		*/
		void SetScene(Scene* scene);

		/**
		 * @brief Get gameObject layer number
		 * @note layer is used physics or gamePlay, layer number must lessThan int bit number (32)
		 * @return 
		*/
		unsigned char GetLayer() { return m_layer; }

		/**
		 * @brief Return gameObject layer number
		 * @note layer is used physics or gamePlay, layer number must lessThan int bit number (32)
		 * @param layer 
		*/
		void SetLayer(unsigned char layer) { m_layer = layer; }

		/**
		 * @brief Set gameObject is active, Also sets whether all child objects are active or inactive
		 * @note if active = true, render system can render this gameObject
		*/
		void SetActive(bool active);

		/**
		 * @brief Get is active
		 * @return 
		*/
		bool GetActive() { return m_active; }

		/**
		 * @brief Set parent GameObject
		 * @param parent 
		 * @return 
		*/
		bool SetParent(GameObject* parent);

		/**
		 * @brief This GameObject has parent
		 * @return 
		*/
		bool HasParent();

		/**
		 * @brief Get this GameObject parent
		 * @return 
		*/
		GameObject* GetParent();

		/**
		 * @brief Get this GameObject child GameObjects
		 * @return 
		*/
		std::list<GameObject*> GetChildren();

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
		/**
		 * @brief Initialize gameObject, call in Constructor
		 */
		void Initialize();

		/**
		 * @brief UnInitialize gameObject, call in Destructor
		*/
		void UnInitialize();

		/**
		 * @brief Recursive active child gameObjects
		*/
		void RecursiveActiveUpdate();

		/**
		 * @brief Check need recursive active child gameObjects
		*/
		void RecursiveWasActiveUpdate();

	public:

		/**
		 * @brief Defines if the actor is sleeping or not.
		 * A sleeping actor will not trigger methods suchs as OnEnable, OnDisable and OnDestroyed
		 * @param p_sleeping
		*/
		void SetSleeping(bool p_sleeping);

		/**
		 * @brief Called when the scene start or when the actor gets enabled for the first time during play mode
		 * This method will always be called in an ordered triple:
		 * - OnAwake()
		 * - OnEnable()
		 * - OnStart()
		*/
		void OnAwake();

		/**
		 * @brief Called when the actor gets enabled (SetActive set to true) or at scene start if the actor is hierarchically active.
		 * This method can be called in an ordered triple at scene start:
		 * - OnAwake()
		 * - OnEnable()
		 * - OnStart()
		 * Or can be called solo if the actor hierarchical active state changed to true and the actor already gets awaked
		 * Conditions:
		 *	- Play mode only
		*/
		void OnEnable();

		/**
		 * @brief Called when the scene start or when the actor gets enabled for the first time during play mode
		 * This method will always be called in an ordered triple:
		 * - OnAwake()
		 * - OnEnable()
		 * - OnStart()
		*/
		void OnStart();

		/**
		 * @brief  Called when the actor hierarchical active state changed to false or gets destroyed while being hierarchically active
		 * Conditions:
		 * - Play mode only
		*/
		void OnDisable();

		/**
		 * @brief  Called when the actor gets destroyed if it has been awaked
		 * Conditions:
		 * - Play mode only
		*/
		void OnDestroy();

		/**
		 * @brief Called every frame
		*/
		void OnUpdate();

		/**
		 * @brief Called every physics frame
		*/
		void OnFixedUpdate();

		/**
		 * @brief Called every frame after OnUpdate
		*/
		void OnLateUpdate();

		/**
		 * @brief Called when the actor enter in collision with another physical object
		 * @param p_otherObject 
		*/
		void OnCollisionEnter(Collider* p_otherObject);

		/**
		 * @brief Called when the actor is in collision with another physical object
		 * @param p_otherObject 
		*/
		void OnCollisionStay(Collider* p_otherObject);

		/**
		 * @brief Called when the actor exit from collision with another physical object
		 * @param p_otherObject 
		*/
		void OnCollisionExit(Collider* p_otherObject);

		/**
		 * @brief Called when the actor enter in trigger with another physical object
		 * @param p_otherObject 
		*/
		void OnTriggerEnter(Collider* p_otherObject);

		/**
		 * @brief Called when the actor is in trigger with another physical object
		 * @param p_otherObject 
		*/
		void OnTriggerStay(Collider* p_otherObject);

		/**
		 * @brief Called when the actor exit from trigger with another physical object
		 * @param p_otherObject 
		*/
		void OnTriggerExit(Collider* p_otherObject);

		/**
		 * @brief call in editor not play mode
		*/
		void OnEditorUpdate();

	public:
		/**
		 * @brief Add Component
		 * @tparam T Component Type
		 * @return Added Component pointer
		*/
		template <class T = Component>
		T* AddComponent();

		/**
		 * @brief Add Component
		 * @tparam T Component Type
		 * @param component Added Component pointer
		*/
		template <class T = Component>
		void AttachComponent(T* component);

		/**
		 * @brief Get Component
		 * @tparam T Component Type
		 * @return return Component pointer
		*/
		template <class T = Component>
		T* GetComponent() const;


		/**
		 * @brief Get Component by unmanagedId
		 * @tparam T Component Type
		 * @param unmanagedId unmanagedId is defined by Script engine 
		 * @return return Component pointer
		*/
		template <class T = Component>
		T* GetComponent(const uint64_t unmanagedId);

		/**
		 * @brief Get GameObject All Component
		 * @return Component List Ref
		*/
		std::vector<Component*>& GetComponents() { return m_componentList; }

		/**
		 * @brief Foreach all Component, every component execute func
		 * @param func execute function, type = std::function
		*/
		void ForeachComponent(std::function<void(Component*)> func);

		/**
		 * @brief Remove a component
		 * @param component Removed component pointer
		 * @return return true if remove success
		*/
		bool RemoveComponent(Component* component);

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		virtual void PostResourceLoaded() override;

		/**
		 * @brief Unique id within the same scene
		 * @note this field will be save local file by Serialization
		*/
		int64_t m_id{0};

		/**
		 * @brief Parent unique id
		 * record GameObjects hierarchy tree
		 * @note this field will be save local file by Serialization
		*/
		int64_t m_parentId{0};

		/**
		 * @brief componentList
		*/
		std::vector<Component*> m_componentList;

		RTTR_ENABLE()

	private:

		/**
		 * @brief This GameObject belong scene pointer
		 * @note if null, not scene gameObject, maybe used to prefab
		*/
		Scene* m_scene = nullptr;

		/**
		 * @brief This GameObject active state
		*/
		bool m_active = true;

		/**
		 * @brief Is playing mode, not editor mode
		*/
		bool m_isPlaying{ false };

		/* Internal settings */
		unsigned char m_layer{0};
		bool	m_destroyed = false;
		bool	m_sleeping = true; // is editor model sleep is true, so is default equal true
		bool	m_awaked = false;
		bool	m_started = false;
		bool	m_wasActive = false;
	};
}

#include "GameObject.inl"
