
#pragma once

#include "RigidActor.h"

namespace LitchiRuntime
{
    class Collider;

    /**
     * @brief RigidStatic Component
     * @note Represents a rigid body that cannot move
    */
    class RigidStatic : public RigidActor {
    public:

        /**
         * @brief Default Constructor
        */
        RigidStatic();

        /**
         * @brief Default Destructor
        */
        ~RigidStatic() override;

    public:

        /**
         * @brief Called when the scene start right before OnStart
         * It allows you to apply prioritized game logic on scene start
        */
        void OnAwake() override;

        /**
         * @brief Called every frame
        */
        void OnUpdate() override;

        /**
         * @brief Called every physics frame
        */
        void OnFixedUpdate() override;

        RTTR_ENABLE(RigidActor)
    };
}