
#pragma once

#include "rttr/registration"
#include "collider.h"

using namespace rttr;
namespace LitchiRuntime
{
    class SphereCollider : public Collider {
    public:
        SphereCollider();
        ~SphereCollider();
        float GetRadius()
        {
            return m_radius;
        }
    protected:
        void CreateShape() override;

    private:
        //~zh 球体碰撞器半径
        float m_radius;

        RTTR_ENABLE(Collider);
    };
}
