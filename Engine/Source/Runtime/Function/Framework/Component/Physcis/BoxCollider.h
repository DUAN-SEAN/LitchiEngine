
#pragma once

#include "rttr/registration"
#include "glm.hpp"
#include "collider.h"
#include "glm.hpp"

using namespace rttr;
namespace LitchiRuntime
{
    class BoxCollider : public Collider {
    public:
        BoxCollider();
        ~BoxCollider();
        glm::vec3 GetSize()
        {
            return m_size;
        }

    protected:
        void CreateShape() override;

    private:
        //~zh 碰撞器尺寸
        glm::vec3 m_size;

        RTTR_ENABLE(Collider);
    };
}
