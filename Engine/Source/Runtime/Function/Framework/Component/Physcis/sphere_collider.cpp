
#include "sphere_collider.h"
#include "rttr/registration"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    SphereCollider::SphereCollider() :Collider(), m_radius(1.0f)
    {

    }

    SphereCollider::~SphereCollider() {

    }

    void SphereCollider::CreateShape() {
        if (m_pxShape == nullptr) {
            m_pxShape = Physics::CreateSphereShape(m_radius, m_pxMaterial);
        }
    }
}