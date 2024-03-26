#include "Runtime/Core/pch.h"
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
    Collider::Collider() : m_pxShape(nullptr), m_pxMaterial(nullptr), m_isTrigger(false), m_offset(0.0f) , m_rigidActor(nullptr){

    }

    Collider::~Collider() {
        if (m_pxShape !=nullptr)
        {
            Physics::ReleaseShape(m_pxShape);
        }
    }

    void Collider::OnAwake() {
    }

    void Collider::CreatePhysicMaterial() {
        
        if (m_pxMaterial == nullptr) {
            m_pxMaterial = Physics::CreateMaterial(m_physicMaterial.GetStaticFriction(), m_physicMaterial.GetDynamicFriction(), m_physicMaterial.GetRestitution());
            m_pxMaterial->setRestitutionCombineMode(PxCombineMode::eAVERAGE);
        }
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
        m_rigidActor->DetachColliderShape(this);
        m_pxShape = nullptr;
    }

    void Collider::PostResourceLoaded()
    {
        CreatePhysicMaterial();
        CreateShape();
        UpdateTriggerState();
        RegisterToRigidActor();
    }

    void Collider::PostResourceModify()
    {
        // todo:
    }

    void Collider::UpdateShape()
    {
        auto rigidActor = GetRigidActor();
        if(rigidActor!=nullptr)
        {
            UnRegisterToRigidActor();
        }

        if(m_pxShape)
        {
            Physics::ReleaseShape(m_pxShape);
        }

        CreateShape();
        UpdateTriggerState();
        RegisterToRigidActor();
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

    void Collider::OnUpdate() {

    }

    void Collider::OnFixedUpdate() {

    }
}