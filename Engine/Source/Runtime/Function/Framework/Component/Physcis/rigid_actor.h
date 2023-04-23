﻿
#ifndef INTEGRATE_PHYSX_RIGID_ACTOR_H
#define INTEGRATE_PHYSX_RIGID_ACTOR_H

#include "PxPhysicsAPI.h"
#include "Runtime/Function/Framework/Component/Base/component.h"

using namespace physx;
namespace LitchiRuntime
{
    class Collider;
    class RigidActor : public Component {
    public:
        RigidActor();
        ~RigidActor();

        virtual void AttachColliderShape(Collider* collider);
        virtual void DeAttachColliderShape(Collider* collider);

    public:
        void Awake() override;

    protected:
        PxRigidActor* px_rigid_actor_;

        RTTR_ENABLE(Component)
    };
}

#endif //INTEGRATE_PHYSX_RIGID_ACTOR_H
