#pragma once

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
        PxRigidActor* m_pxRigidActor;

        RTTR_ENABLE(Component)
    };
}
