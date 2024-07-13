
#pragma once

#include "rttr/registration"
#include "PxPhysicsAPI.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Function/Framework/Component/Base/component.h"

using namespace rttr;
using namespace physx;
namespace LitchiRuntime
{
	/**
	 * @brief Physics Material Info
	 * Contains the friction and elasticity coefficients of a physical object
	*/
	class PhysicMaterialRes {
	public:

		/**
		 * @brief Default Constructor
		*/
		PhysicMaterialRes() {}

		/**
		 * @brief Construct from static/dynamic friction and restitution
		 * @param staticFriction
		 * @param dynamicFriction
		 * @param restitution
		*/
		PhysicMaterialRes(float staticFriction, float dynamicFriction, float restitution) : m_staticFriction(staticFriction), m_dynamicFriction(dynamicFriction), m_restitution(restitution) {}

		/**
		 * @brief Set Coefficient of static friction
		 * @return
		*/
		void SetStaticFriction(float staticFriction) { m_staticFriction = staticFriction; }

		/**
		 * @brief Get Coefficient of static friction
		 * @return
		*/
		float GetStaticFriction() { return m_staticFriction; }

		/**
		 * @brief Set Coefficient of dynamic friction
		 * @param dynamicFriction
		*/
		void SetDynamicFriction(float dynamicFriction) { m_dynamicFriction = dynamicFriction; }

		/**
		 * @brief Get Coefficient of dynamic friction
		 * @return
		*/
		float GetDynamicFriction() { return m_dynamicFriction; }

		/**
		 * @brief Set Coefficient of restitution
		 * @param restitution
		*/
		void SetRestitution(float restitution) { m_restitution = restitution; }

		/**
		 * @brief Get Coefficient of restitution
		 * @return
		*/
		float GetRestitution() { return m_restitution; }

	private:
		float m_staticFriction = 0.6f;
		float m_dynamicFriction = 0.6f;
		float m_restitution = 0.1f;
	};

	class RigidActor;

	/**
	 * @brief Collider Component
	*/
	class Collider : public Component {
	public:

		/**
		 * @brief Default Constructor
		*/
		Collider();

		/**
		 * @brief Default Destructor
		*/
		~Collider() override;

		/**
		 * @brief Set PhysicMaterial
		 * @param physicMaterialRes
		*/
		void SetPhysicMaterial(PhysicMaterialRes physicMaterialRes)
		{
			m_physicMaterial = physicMaterialRes;
		}

		/**
		 * @brief Get PhysicMaterial
		 * @return
		*/
		PhysicMaterialRes GetPhysicMaterial()
		{
			return m_physicMaterial;
		}

		/**
		 * @brief Set this Collider is trigger
		 * @note Triggers can only collide with non-triggers
		 * @param isTrigger
		*/
		void SetIsTrigger(bool isTrigger) {
			if (m_isTrigger == isTrigger) {
				return;
			}
			m_isTrigger = isTrigger;
			UpdateShape();
		}

		/**
		 * @brief Check this collider is trigger
		 * @note Triggers can only collide with non-triggers
		 * @return
		*/
		bool IsTrigger() { return m_isTrigger; }

		/**
		 * @brief Get physX PxShape pointer
		 * @return
		*/
		PxShape* GetPxShape() { return m_pxShape; }


		/**
		 * @brief Set Collider offset
		 * @param offset
		*/
		void SetOffset(Vector3 offwwwwwwwwwwwset)
		{
			m_offset = offset;
		}

		/**
		 * @brief Get Collider Offset
		 * @return
		*/
		Vector3 GetOffset()
		{
			return m_offset;
		}

	public:

		/**
		 * @brief Called when the scene start right before OnStart
		 * It allows you to apply prioritized game logic on scene start
	    */
		void OnAwake() override;

		/**
         * @brief Called every frame
        */
		void OnUpdate() override;

		/**
		 * @brief Called every physics frame
		*/
		void OnFixedUpdate() override;

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		void PostResourceLoaded() override;

	protected:

		/**
		 * @brief Create PhysX Shape
		 * @note This is abstract function, derived must implement
		*/
		virtual void CreateShape() = 0;

		/**
		 * @brief Create PhysicMaterial
		*/
		virtual void CreatePhysicMaterial();

		/**
		 * @brief Update trigger state
		*/
		virtual void UpdateTriggerState();

		/**
		 * @brief Register this Collider to physX rigidActor
		*/
		virtual void RegisterToRigidActor();

		/**
		 * @brief Unregister this Collider to physX rigidActor
		*/
		virtual void UnRegisterToRigidActor();

		/**
		 * @brief Update current physX Shape
		*/
		void UpdateShape();

		/**
		 * @brief Get PhysX RigidActor pointer
		 * @return
		*/
		RigidActor* GetRigidActor();

		/**
		 * @brief PhysX PxShape pointer
		*/
		PxShape* m_pxShape = nullptr;

		/**
		 * @brief PhysX PxMaterial pointer
		*/
		PxMaterial* m_pxMaterial = nullptr;

		/**
		 * @brief Trigger collider
		 * @note Triggers can only collide with non-triggers
		*/
		bool m_isTrigger = false;

		/**
		 * @brief Relative local transform
		*/
		Vector3 m_offset{0.0f};

		/**
		 * @brief PhysX PxMaterial pointer
		*/
		RigidActor* m_rigidActor = nullptr;

		/**
		 * @brief PhysicMaterialRes
		*/
		PhysicMaterialRes m_physicMaterial;

		RTTR_ENABLE(Component);
	};
}
