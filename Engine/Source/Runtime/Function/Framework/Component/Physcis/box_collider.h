
#ifndef INTEGRATE_PHYSX_BOX_COLLIDER_H
#define INTEGRATE_PHYSX_BOX_COLLIDER_H

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
            return size_;
        }

    protected:
        void CreateShape() override;

    private:
        //~zh 碰撞器尺寸
        glm::vec3 size_;

        RTTR_ENABLE(Collider);
    };
}


#endif //INTEGRATE_PHYSX_BOX_COLLIDER_H
