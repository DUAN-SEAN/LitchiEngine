
#ifndef INTEGRATE_PHYSX_COLLIDER_H
#define INTEGRATE_PHYSX_COLLIDER_H

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
        PhysicMaterialRes(float static_friction, float dynamic_friction, float restitution) : static_friction_(static_friction), dynamic_friction_(dynamic_friction), restitution_(restitution) {}

        float static_friction()  { return static_friction_; }
        void set_static_friction(float static_friction) { static_friction_ = static_friction; }

        float dynamic_friction()  { return dynamic_friction_; }
        void set_dynamic_friction(float dynamic_friction) { dynamic_friction_ = dynamic_friction; }

        float restitution()  { return restitution_; }
        void set_restitution(float restitution) { restitution_ = restitution; }

    private:
        float static_friction_;
        float dynamic_friction_;
        float restitution_;
    };

    class RigidActor;
    class Collider : public Component {
    public:
        Collider();
        ~Collider();

        PxShape* px_shape() { return px_shape_; }

        bool Istrigger() { return is_trigger_; }
        void SetIsTrigger(bool is_trigger) {
            if (is_trigger_ == is_trigger) {
                return;
            }
            is_trigger_ = is_trigger;
            UnRegisterToRigidActor();
            CreateShape();
            UpdateTriggerState();
            RegisterToRigidActor();
        }

    public:
        void SetPhysicMaterial(PhysicMaterialRes physic_material_res)
        {
            physic_material_ = physic_material_res;
        }

        PhysicMaterialRes GetPhysicMaterial()
        {
        	return physic_material_;
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
        PxShape* px_shape_;
        PxMaterial* px_material_;
        bool is_trigger_;//是触发器，触发器只检测碰撞，而不进行物理模拟。

    private:
        RigidActor* rigid_actor_;

    private:
        PhysicMaterialRes physic_material_;
   
        RTTR_ENABLE();
    };
}


#endif //INTEGRATE_PHYSX_COLLIDER_H
