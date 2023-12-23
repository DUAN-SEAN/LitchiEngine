
#include "RigidStatic.h"

#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "collider.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    RigidStatic::RigidStatic() {

    }

    RigidStatic::~RigidStatic() {

    }

    void RigidStatic::Awake() {
        Transform* transform = GetGameObject()->GetComponent<Transform>();
        PxRigidStatic* px_rigid_static_ = Physics::CreateRigidStatic(transform->GetPosition(), transform->GetRotation(), GetGameObject()->GetName().c_str());
        m_pxRigidActor = dynamic_cast<PxRigidActor*>(px_rigid_static_);
        RigidActor::Awake();
    }

    void RigidStatic::Update() {

    }

    void RigidStatic::FixedUpdate() {
    }
}