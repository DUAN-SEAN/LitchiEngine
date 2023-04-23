
#include "sphere_collider.h"
#include "rttr/registration"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    SphereCollider::SphereCollider() :Collider(), radius_(1.0f)
    {

    }

    SphereCollider::~SphereCollider() {

    }

    void SphereCollider::CreateShape() {
        if (px_shape_ == nullptr) {
            px_shape_ = Physics::CreateSphereShape(radius_, px_material_);
        }
    }
}