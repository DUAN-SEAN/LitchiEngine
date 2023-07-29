
#pragma once

#include "RigidActor.h"

namespace LitchiRuntime
{
    class Collider;
    class RigidStatic : public RigidActor {
    public:
        RigidStatic();
        ~RigidStatic();

    public:
        /// Awake里反序列化给成员变量赋值。
        void Awake() override;

        void Update() override;

        void FixedUpdate() override;

        RTTR_ENABLE(RigidActor)
    };
}