
#pragma once

//= INCLUDES ===================
#include "Vector3.h"
#include "../Definitions.h"
//==============================

namespace LitchiRuntime
{
   /* namespace Math
    {*/
        class RayHit;
        class BoundingBox;
        class Plane;
        class Sphere;

        class SP_CLASS Ray
        {
        public:
            Ray() = default;
            Ray(const Vector3& start, const Vector3& direction);
            ~Ray() = default;

            // Returns hit distance or infinity if there is no hit
            float HitDistance(const BoundingBox& box) const;
            float HitDistance(const Plane& plane, Vector3* intersection_point = nullptr) const;
            float HitDistance(const Vector3& v1, const Vector3& v2, const Vector3& v3, Vector3* out_normal = nullptr, Vector3* out_bary = nullptr) const;
            float HitDistance(const Sphere& sphere) const;

            // Returns the distance
            float Distance(const Vector3& point) const;
            float Distance(const Vector3& point, Vector3& closest_point) const;

            // Returns the closest point
            Vector3 ClosestPoint(const Ray& ray) const;

            const Vector3& GetStart()     const { return m_origin; }
            const Vector3& GetDirection() const { return m_direction; }
            bool IsDefined()              const { return m_origin != m_direction && m_direction != Vector3::Zero; }

        private:
            Vector3 m_origin    = Vector3::Zero;
            Vector3 m_direction = Vector3::Zero;
        };
    //}
}
