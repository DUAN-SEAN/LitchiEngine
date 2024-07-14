
#include "Runtime/Core/pch.h"
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
        UpdateShapeRadius(m_radius);
    }

    void SphereCollider::UpdateShapeRadius(float radius)
    {
        m_radius = Math::Helper::Max(radius, 0.05f);

        UpdateShape();
    }

    void SphereCollider::CreateShape() {
        if (m_pxShape == nullptr) {
            m_pxShape = Physics::CreateSphereShape(m_radius, m_pxMaterial, m_offset, Quaternion::Identity);
        }
    }
}