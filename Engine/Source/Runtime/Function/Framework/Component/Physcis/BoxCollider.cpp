#include "Runtime/Core/pch.h"
#include "BoxCollider.h"

#include "rttr/registration"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Physics/physics.h"

using namespace rttr;
namespace LitchiRuntime
{
    BoxCollider::BoxCollider() :Collider(), m_size(1, 1, 1)
    {

    }

    BoxCollider::~BoxCollider() {

    }

    void BoxCollider::UpdateShapeSize(const Vector3& size)
    {
        m_size = size;
        UpdateShape();
    }

    void BoxCollider::PostResourceModify()
    {
        Collider::PostResourceModify();

        UpdateShapeSize(m_size);
    }

    void BoxCollider::CreateShape() {
        if (m_pxShape == nullptr) {
            m_pxShape = Physics::CreateBoxShape(m_size, m_pxMaterial,m_offset, Quaternion::Identity);
        }
    }
}