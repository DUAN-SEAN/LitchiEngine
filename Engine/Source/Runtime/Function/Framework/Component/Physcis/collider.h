
#pragma once

#include "rttr/registration"
#include "PxPhysicsAPI.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Function/Framework/Component/Base/component.h"

using namespace rttr;
using namespace physx;
namespace LitchiRuntime
{

    class PhysicMaterialRes {
    public:
        PhysicMaterialRes() {}
        PhysicMaterialRes(float static_friction, float dynamic_friction, float restitution) : m_staticFriction(static_friction), m_dynamicFriction(dynamic_friction), m_restitution(restitution) {}

        float GetStaticFriction()  { return m_staticFriction; }
        void SetStaticFriction(float static_friction) { m_staticFriction = static_friction; }

        float GetDynamicFriction()  { return m_dynamicFriction; }
        void SetDynamicFriction(float dynamic_friction) { m_dynamicFriction = dynamic_friction; }

        float GetRestitution()  { return m_restitution; }
        void SetRestitution(float restitution) { m_restitution = restitution; }

    private:
        float m_staticFriction = 0.6f;
        float m_dynamicFriction = 0.6f;
        float m_restitution = 0.1f;
    };

    class RigidActor;
    class Collider : public Component {
    public:
        Collider();
        ~Collider();

        PxShape* px_shape() { return m_pxShape; }

        bool Istrigger() { return m_isTrigger; }
        void SetIsTrigger(bool is_trigger) {
            if (m_isTrigger == is_trigger) {
                return;
            }
            m_isTrigger = is_trigger;
            UpdateShape();
        }

    public:
        void SetPhysicMaterial(PhysicMaterialRes physic_material_res)
        {
            m_physicMaterial = physic_material_res;
        }

        PhysicMaterialRes GetPhysicMaterial()
        {
        	return m_physicMaterial;
		}

    public:
        /// Awake里反序列化给成员变量赋值。
        void Awake() override;

        void Update() override;

        void FixedUpdate() override;

        virtual void CreatePhysicMaterial();

        void PostResourceLoaded() override;
    	void PostResourceModify() override;
    protected:
        virtual void CreateShape();
        /// 更新Shape 触发器 Filter
        virtual void UpdateTriggerState();
        virtual void RegisterToRigidActor();
        virtual void UnRegisterToRigidActor();

        void UpdateShape();

    private:
        RigidActor* GetRigidActor();

    protected:
        PxShape* m_pxShape;
        PxMaterial* m_pxMaterial;
        bool m_isTrigger;// trigger collider not simulation
        Vector3 m_offset;// relative local transform

    private:
        RigidActor* m_rigidActor;
        PhysicMaterialRes m_physicMaterial;

        RTTR_ENABLE(Component);
    };
}
