#pragma once

#include "RigidActor.h"

namespace LitchiRuntime
{
    class Collider;
    class RigidDynamic : public RigidActor {
    public:
        RigidDynamic();
        ~RigidDynamic();

        void UpdateCCDState();

        bool GetEnableCCD() { return m_enableCCD; }
        void SetEnableCCD(bool enable_ccd) {
            m_enableCCD = enable_ccd;
            UpdateCCDState();
        }

    public:
        /// Awake里反序列化给成员变量赋值。
        void OnAwake() override;

        void OnUpdate() override;

        void OnFixedUpdate() override;

    private:
        bool m_enableCCD;

        RTTR_ENABLE(RigidActor)
    };
}
