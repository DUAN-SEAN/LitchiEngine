
#include "RigidActor.h"

#include "rttr/registration"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Core/Log/debug.h"
#include "collider.h"
#include "Runtime/Function/Physics/physics.h"

namespace LitchiRuntime
{
    RigidActor::RigidActor() : m_pxRigidActor(nullptr) {

    }

    RigidActor::~RigidActor() {

        if (m_pxRigidActor != nullptr)
        {
            // if attach shape, internal release shape ref count, if shape ref = 0 shape will be released
            Physics::ReleaseRigidActor(m_pxRigidActor);
        }
    }

    void RigidActor::Awake() {
        Component* component = GetGameObject()->GetComponent<Collider>();
        if (component != nullptr) {
            Collider* collider = static_cast<Collider*>(component);
            AttachColliderShape(collider);
        }
    }

    void RigidActor::AttachColliderShape(Collider* collider) {
        if (collider == nullptr) {
            return;
        }
        PxShape* px_shape = collider->px_shape();
        Physics::AttachShape(m_pxRigidActor, px_shape);
        //    DEBUG_LOG_INFO("px_shape refCount: {}",px_shape->getReferenceCount());
    }

    void RigidActor::DetachColliderShape(Collider* collider) {
        if (collider == nullptr) {
            return;
        }
        PxShape* px_shape = collider->px_shape();
        Physics::DetachShape(m_pxRigidActor, px_shape);
    }
}
