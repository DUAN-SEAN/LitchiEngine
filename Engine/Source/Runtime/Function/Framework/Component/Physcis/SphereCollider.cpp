
#include "SphereCollider.h"

#include "rttr/registration"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    SphereCollider::SphereCollider() :Collider(), m_radius(1.0f)
    {

    }

    SphereCollider::~SphereCollider() {

    }

    void SphereCollider::UpdateRadius(float radius)
    {
        if(m_pxShape)
        {
            Physics::UpdateSphereShapeSize(m_pxShape, radius);
        }
    }

    void SphereCollider::CreateShape() {
        if (m_pxShape == nullptr) {
            m_pxShape = Physics::CreateSphereShape(m_radius, m_pxMaterial);
        }
    }
}