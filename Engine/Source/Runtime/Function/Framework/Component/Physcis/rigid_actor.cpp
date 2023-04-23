
#include "rigid_actor.h"
#include "rttr/registration"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Core/Log/debug.h"
#include "collider.h"
namespace LitchiRuntime
{
    RigidActor::RigidActor() : px_rigid_actor_(nullptr) {

    }

    RigidActor::~RigidActor() {

    }

    void RigidActor::Awake() {
        Component* component = game_object()->GetComponent<Collider>();
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
        px_rigid_actor_->attachShape(*px_shape);
        px_shape->release();
        //    DEBUG_LOG_INFO("px_shape refCount: {}",px_shape->getReferenceCount());
    }

    void RigidActor::DeAttachColliderShape(Collider* collider) {
        if (collider == nullptr) {
            return;
        }
        PxShape* px_shape = collider->px_shape();
        px_rigid_actor_->detachShape(*px_shape);//detach后px_shape refCount==0，自动释放。
    }
}