#pragma once

#include "PxPhysicsAPI.h"
#include "Runtime/Function/Framework/Component/Base/component.h"

using namespace physx;
namespace LitchiRuntime
{
    class Collider;

    /**
     * @brief RigidActor Component
    */
    class RigidActor : public Component {
    public:

        /**
         * @brief Default Constructor
        */
        RigidActor();

        /**
         * @brief Default Destructor
        */
        ~RigidActor() override;

        /**
         * @brief Attach Shape to this RigidActor
         * @param collider 
        */
        virtual void AttachColliderShape(Collider* collider);

        /**
         * @brief Detach Shape from this RigidActor
         * @param collider 
        */
        virtual void DetachColliderShape(Collider* collider);

    public:

        /**
         * @brief Called when the scene start right before OnStart
         * It allows you to apply prioritized game logic on scene start
        */
        void OnAwake() override;

    protected:

        /**
         * @brief PhysX PxRigidActor pointer
        */
        PxRigidActor* m_pxRigidActor;

        RTTR_ENABLE(Component)
    };
}
