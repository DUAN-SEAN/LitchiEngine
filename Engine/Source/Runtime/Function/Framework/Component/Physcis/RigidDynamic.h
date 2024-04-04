#pragma once

#include "RigidActor.h"

namespace LitchiRuntime
{
    class Collider;

    /**
     * @brief RigidDynamic Component
     * @note Represents a rigid body that can move
    */
    class RigidDynamic : public RigidActor {
    public:

        /**
         * @brief Default Constructor
        */
        RigidDynamic();

        /**
         * @brief Default Destructor
        */
        ~RigidDynamic() override;

        /**
         * @brief Set enable CCD
         * @param enable_ccd 
        */
        void SetEnableCCD(bool enable_ccd) {
            m_enableCCD = enable_ccd;
            UpdateCCDState();
        }

        /**
         * @brief Get enable CCD
         * @return 
        */
        bool GetEnableCCD() { return m_enableCCD; }
     

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

    private:

        /**
         * @brief Update CCD State
        */
        void UpdateCCDState();

        /**
         * @brief open "Continuous collision detection" mode
        */
        bool m_enableCCD;

        RTTR_ENABLE(RigidActor)
    };
}
