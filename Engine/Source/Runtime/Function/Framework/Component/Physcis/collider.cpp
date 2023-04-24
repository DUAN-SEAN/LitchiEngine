
#include "collider.h"

#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "rigid_dynamic.h"
#include "rigid_static.h"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    Collider::Collider() : px_shape_(nullptr), px_material_(nullptr), rigid_actor_(nullptr), is_trigger_(false) {

    }

    Collider::~Collider() {

    }

    void Collider::Awake() {
        CreatePhysicMaterial();
        CreateShape();
        UpdateTriggerState();
        RegisterToRigidActor();
    }

    void Collider::CreatePhysicMaterial() {
        
        if (px_material_ == nullptr) {
            px_material_ = Physics::CreateMaterial(physic_material_.static_friction(), physic_material_.dynamic_friction(), physic_material_.restitution());
            px_material_->setRestitutionCombineMode(PxCombineMode::eAVERAGE);
        }
    }

    void Collider::CreateShape() {

    }

    void Collider::UpdateTriggerState() {
        if (px_shape_ == nullptr) {
            return;
        }
        //~zh 设置附加数据为1，表示当前Shape是Trigger
        //~en set shape's user data 1, it is a trigger.
        px_shape_->setSimulationFilterData(PxFilterData(is_trigger_ ? 1 : 0, 0, 0, 0));
        px_shape_->userData = game_object();
    }

    void Collider::RegisterToRigidActor() {
        if (GetRigidActor() == nullptr) {
            DEBUG_LOG_ERROR("rigid_actor_ is nullptr,Collider need to be attached to a rigid_actor");
            return;
        }
        rigid_actor_->AttachColliderShape(this);
    }

    void Collider::UnRegisterToRigidActor() {
        if (GetRigidActor() == nullptr) {
            DEBUG_LOG_ERROR("rigid_actor_ is nullptr,Collider need to be attached to a rigid_actor");
            return;
        }
        rigid_actor_->DeAttachColliderShape(this);
        px_shape_ = nullptr;
    }

    RigidActor* Collider::GetRigidActor() {
        if (rigid_actor_ != nullptr) {
            return rigid_actor_;
        }
        rigid_actor_ = game_object()->GetComponent<RigidDynamic>();
        if (rigid_actor_ != nullptr) {
            return rigid_actor_;
        }
        rigid_actor_ = game_object()->GetComponent<RigidStatic>();
        if (rigid_actor_ != nullptr) {
            return rigid_actor_;
        }
        return nullptr;
    }

    void Collider::Update() {

    }

    void Collider::FixedUpdate() {

    }
}