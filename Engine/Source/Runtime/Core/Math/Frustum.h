
#pragma once

//= INCLUDES =============
#include "../Math/Plane.h"
#include "Matrix.h"
#include "Vector3.h"
//========================

namespace LitchiRuntime
{
    class Frustum
    {
    public:
        Frustum() = default;
        Frustum(const Matrix& mView, const Matrix& mProjection, float screenDepth);
        ~Frustum() = default;

        bool IsVisible(const Vector3& center, const Vector3& extent, bool ignore_near_plane = false) const;

    private:
        Intersection CheckCube(const Vector3& center, const Vector3& extent) const;
        Intersection CheckSphere(const Vector3& center, float radius) const;

        Plane m_planes[6];
    };
}
