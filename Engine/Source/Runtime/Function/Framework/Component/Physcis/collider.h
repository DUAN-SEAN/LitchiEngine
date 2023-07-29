
#pragma once

#include "rttr/registration"
#include "PxPhysicsAPI.h"
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
        float m_staticFriction;
        float m_dynamicFriction;
        float m_restitution;
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
            UnRegisterToRigidActor();
            CreateShape();
            UpdateTriggerState();
            RegisterToRigidActor();
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
        virtual void CreateShape();
        /// 更新Shape 触发器 Filter
        virtual void UpdateTriggerState();
        virtual void RegisterToRigidActor();
        virtual void UnRegisterToRigidActor();

    private:
        RigidActor* GetRigidActor();

    protected:
        PxShape* m_pxShape;
        PxMaterial* m_pxMaterial;
        bool m_isTrigger;//是触发器，触发器只检测碰撞，而不进行物理模拟。

    private:
        RigidActor* m_rigidActor;

    private:
        PhysicMaterialRes m_physicMaterial;
   
        RTTR_ENABLE(Component);
    };
}
