
#include "BoxCollider.h"

#include "rttr/registration"
#include "Runtime/Function/Physics/physics.h"

using namespace rttr;
namespace LitchiRuntime
{


    BoxCollider::BoxCollider() :Collider(), m_size(1, 1, 1)
    {

    }

    BoxCollider::~BoxCollider() {

    }

    void BoxCollider::UpdateSize(const Vector3& size)
    {
        if(m_pxShape)
        {
            Physics::UpdateBoxShapeSize(m_pxShape, size);
        }
    }

    void BoxCollider::CreateShape() {
        if (m_pxShape == nullptr) {
            m_pxShape = Physics::CreateBoxShape(m_size, m_pxMaterial);
        }
    }
}