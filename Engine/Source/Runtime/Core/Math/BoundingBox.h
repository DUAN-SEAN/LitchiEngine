
#pragma once

//= INCLUDES ==========
#include "MathHelper.h"
#include "Vector3.h"
//=====================

namespace LitchiRuntime
{
    struct RHI_Vertex_PosTexNorTan;
    struct RHI_Vertex_PosTexNorTanBone;

   /* namespace Math
    {*/
        class Matrix;

        class LC_CLASS BoundingBox
        {
        public:
            // Construct with zero size.
            BoundingBox();

            // Construct from minimum and maximum vectors.
            BoundingBox(const Vector3& min, const Vector3& max);

            // Construct from points
            BoundingBox(const Vector3* vertices, const uint32_t point_count);

            // Construct from vertices
            BoundingBox(const RHI_Vertex_PosTexNorTan* vertices, const uint32_t vertex_count);
            BoundingBox(const RHI_Vertex_PosTexNorTanBone* vertices, const uint32_t vertex_count);

            ~BoundingBox() = default;

            // Assign from bounding box
            BoundingBox& operator =(const BoundingBox& rhs) = default;

            // Returns the center
            Vector3 GetCenter() const { return (m_max + m_min) * 0.5f; }

            // Returns the size
            Vector3 GetSize() const { return m_max - m_min; }

            // Returns extents
            Vector3 GetExtents() const { return (m_max - m_min) * 0.5f; }

            // Test if a point is inside
            Intersection IsInside(const Vector3& point) const;

            // Test if a bounding box is inside
            Intersection IsInside(const BoundingBox& box) const;

            // Returns a transformed bounding box
            BoundingBox Transform(const Matrix& transform) const;

            // Merge with another bounding box
            void Merge(const BoundingBox& box);

            bool operator==(const BoundingBox& other) const
            {
                return GetMin() == other.GetMin() && GetMax() == other.GetMax();
            }

            const Vector3& GetMin() const { return m_min; }
            const Vector3& GetMax() const { return m_max; }

            static const BoundingBox Undefined;

        private:
            Vector3 m_min;
            Vector3 m_max;
        };
    //}
}
