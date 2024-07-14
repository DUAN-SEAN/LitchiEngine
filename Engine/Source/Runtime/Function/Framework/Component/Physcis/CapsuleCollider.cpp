
#include "Runtime/Core/pch.h"
#include "CapsuleCollider.h"

#include "rttr/registration"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    CapsuleCollider::CapsuleCollider() :Collider(), m_radius(1.0f),m_height(2.0f)
    {

    }

    CapsuleCollider::~CapsuleCollider() {

    }

    void CapsuleCollider::PostResourceModify()
    {
        Collider::PostResourceLoaded();
        UpdateShapeRadiusHeight(m_radius,m_height);
    }

    void CapsuleCollider::UpdateShapeRadiusHeight(float radius, float height)
    {
        m_radius = Math::Helper::Max(radius,0.05f);
        m_height = Math::Helper::Max(height,0.05f);
        
        UpdateShape();
    }

    void CapsuleCollider::CreateShape() {
        if (m_pxShape == nullptr) {
            m_pxShape = Physics::CreateCapsuleShape(m_radius, m_height / 2.0f,m_pxMaterial, m_offset, Quaternion::Identity);
        }
    }
}