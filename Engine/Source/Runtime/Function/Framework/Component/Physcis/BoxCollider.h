﻿
#pragma once

#include "rttr/registration"
#include "collider.h"
#include "Runtime/Core/Math/Vector3.h"

using namespace rttr;
namespace LitchiRuntime
{
    class BoxCollider : public Collider {
    public:
        BoxCollider();
        ~BoxCollider();
        Vector3 GetSize()
        {
            return m_size;
        }

        void SetSize(Vector3 size)
        {
            m_size = size;
        }

        void UpdateSize(const Vector3& size);

        void PostResourceLoaded() override;
        void PostResourceModify() override;

    protected:
        void CreateShape() override;

    private:
        //~zh 碰撞器尺寸
        Vector3 m_size;

        RTTR_ENABLE(Collider);
    };
}
