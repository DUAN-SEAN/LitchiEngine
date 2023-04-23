
#include "box_collider.h"
#include "rttr/registration"
#include "Runtime/Function/Physics/physics.h"

using namespace rttr;
namespace LitchiRuntime
{


    BoxCollider::BoxCollider() :Collider(), size_(1, 1, 1)
    {

    }

    BoxCollider::~BoxCollider() {

    }

    void BoxCollider::CreateShape() {
        if (px_shape_ == nullptr) {
            px_shape_ = Physics::CreateBoxShape(size_, px_material_);
        }
    }
}