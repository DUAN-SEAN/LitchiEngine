
#include "collider.h"

#include "RigidActor.h"
#include "RigidDynamic.h"
#include "RigidStatic.h"
#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Physics/physics.h"

namespace LitchiRuntime
{
    Collider::Collider() : m_pxShape(nullptr), m_pxMaterial(nullptr), m_rigidActor(nullptr), m_isTrigger(false) {

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
        
      /*  if (px_material_ == nullptr) {
            px_material_ = Physics::CreateMaterial(physic_material_.static_friction(), physic_material_.dynamic_friction(), physic_material_.restitution());
            px_material_->setRestitutionCombineMode(PxCombineMode::eAVERAGE);
        }*/
    }

    void Collider::CreateShape() {

    }

    void Collider::UpdateTriggerState() {
        if (m_pxShape == nullptr) {
            return;
        }
        //~zh 设置附加数据为1，表示当前Shape是Trigger
        //~en set shape's user data 1, it is a trigger.
        m_pxShape->setSimulationFilterData(PxFilterData(m_isTrigger ? 1 : 0, 0, 0, 0));
        m_pxShape->userData = GetGameObject();
    }

    void Collider::RegisterToRigidActor() {
        if (GetRigidActor() == nullptr) {
            DEBUG_LOG_ERROR("rigid_actor_ is nullptr,Collider need to be attached to a rigid_actor");
            return;
        }
        m_rigidActor->AttachColliderShape(this);
    }

    void Collider::UnRegisterToRigidActor() {
        if (GetRigidActor() == nullptr) {
            DEBUG_LOG_ERROR("rigid_actor_ is nullptr,Collider need to be attached to a rigid_actor");
            return;
        }
        m_rigidActor->DeAttachColliderShape(this);
        m_pxShape = nullptr;
    }

    RigidActor* Collider::GetRigidActor() {
        if (m_rigidActor != nullptr) {
            return m_rigidActor;
        }
        m_rigidActor = GetGameObject()->GetComponent<RigidDynamic>();
        if (m_rigidActor != nullptr) {
            return m_rigidActor;
        }
        m_rigidActor = GetGameObject()->GetComponent<RigidStatic>();
        if (m_rigidActor != nullptr) {
            return m_rigidActor;
        }
        return nullptr;
    }

    void Collider::Update() {

    }

    void Collider::FixedUpdate() {

    }
}