
#pragma once

//= INCLUDES =======
#include "Vector3.h"
//==================

namespace LitchiRuntime
{
    class Plane
    {
    public:
        Plane() = default;

        Plane(const Vector3& normal, float d);

        // Construct from a normal vector and a point on the plane
        Plane(const Vector3& normal, const Vector3& point);

        // Construct from 3 vertices
        Plane(const Vector3& a, const Vector3& b, const Vector3& c);

        ~Plane() = default;

        void Normalize();
        static Plane Normalize(const Plane& plane);

        float Dot(const Vector3& v) const;
        static float Dot(const Plane& p, const Vector3& v);

        Vector3 normal = Vector3::Zero;
        float d = 0.0f; // distance from origin
    };
}
