
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

    void SphereCollider::PostResourceModify()
    {
        Collider::PostResourceLoaded();
        UpdateRadius(m_radius);
    }

    void SphereCollider::UpdateRadius(float radius)
    {
        m_radius = radius;
        UpdateShape();
    }

    void SphereCollider::CreateShape() {
        if (m_pxShape == nullptr) {
            m_pxShape = Physics::CreateSphereShape(m_radius, m_pxMaterial, m_offset, Quaternion::Identity);
        }
    }
}