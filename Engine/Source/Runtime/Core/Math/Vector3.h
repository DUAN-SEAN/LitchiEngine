
#pragma once

//= INCLUDES ===================
#include <string>               
#include "MathHelper.h"         
#include "../Definitions.h"
//==============================

namespace LitchiRuntime
{
    class Vector4;

    class LC_CLASS Vector3
    {
    public:
        // Constructor
        Vector3()
        {
            x = 0;
            y = 0;
            z = 0;
        }

        Vector3(const Vector3& vector) = default;

        // Copy-constructor
        Vector3(const Vector4& vector);

        // Construct from coordinates.
        Vector3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        // Construct from array.
        Vector3(float pos[3])
        {
            this->x = pos[0];
            this->y = pos[1];
            this->z = pos[2];
        }

        // Construct from single value.
        Vector3(float f)
        {
            x = f;
            y = f;
            z = f;
        }
        
        void Normalize()
        {
            const auto length_squared = LengthSquared();
            if (!Math::Helper::Equals(length_squared, 1.0f) && length_squared > 0.0f)
            {
                const auto length_inverted = 1.0f / Math::Helper::Sqrt(length_squared);
                x *= length_inverted;
                y *= length_inverted;
                z *= length_inverted;
            }
        };

        // Return normalized vector
        [[nodiscard]] Vector3 Normalized() const
        {
            Vector3 v = *this;
            v.Normalize();
            return v;
        }

        // Returns normalized vector
        static inline Vector3 Normalize(const Vector3& v) { return v.Normalized(); }

        inline bool IsNormalized() const
        {
            static const float THRESH_VECTOR_NORMALIZED = 0.01f;
            return (Math::Helper::Abs(1.f - LengthSquared()) < THRESH_VECTOR_NORMALIZED);
        }

        inline float Max()
        {
            return Math::Helper::Max3(x, y, z);
        }

        // Returns the dot product
        static inline float Dot(const Vector3& v1, const Vector3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }

        // Returns the dot product
        [[nodiscard]] float Dot(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

        // Returns the cross product
        static inline Vector3 Cross(const Vector3& v1, const Vector3& v2)
        {
            return Vector3(
                v1.y * v2.z - v2.y * v1.z,
                -(v1.x * v2.z - v2.x * v1.z),
                v1.x * v2.y - v2.x * v1.y
            );
        }
        // Returns the cross product
        [[nodiscard]] Vector3 Cross(const Vector3& v2) const { return Cross(*this, v2); }

        // Returns the length
        [[nodiscard]] float Length() const        { return Math::Helper::Sqrt(x * x + y * y + z * z); }
        // Returns the squared length             
        [[nodiscard]] float LengthSquared() const { return x * x + y * y + z * z; }

        // Returns a copy of /vector/ with its magnitude clamped to /maxLength/.
        inline void ClampMagnitude(float max_length)
        {
            const float sqrmag = LengthSquared();

            if (sqrmag > max_length * max_length)
            {
                const float mag = Math::Helper::Sqrt(sqrmag);

                // these intermediate variables force the intermediate result to be
                // of float precision. without this, the intermediate result can be of higher
                // precision, which changes behavior.

                const float normalized_x = x / mag;
                const float normalized_y = y / mag;
                const float normalized_z = z / mag;

                x = normalized_x * max_length;
                y = normalized_y * max_length;
                z = normalized_z * max_length;
            }
        }

        inline void FindBestAxisVectors(Vector3& Axis1, Vector3& Axis2) const
        {
            const float NX = Math::Helper::Abs(x);
            const float NY = Math::Helper::Abs(y);
            const float NZ = Math::Helper::Abs(z);

            // Find best basis vectors.
            if (NZ > NX && NZ > NY)	Axis1 = Vector3(1, 0, 0);
            else                    Axis1 = Vector3(0, 0, 1);

            Axis1 = (Axis1 - *this * (Axis1.Dot(*this))).Normalized();
            Axis2 = Axis1.Cross(*this);
        }

        inline float Distance(const Vector3& x)                                 { return ((*this) - x).Length(); }
        inline float DistanceSquared(const Vector3& x)                          { return ((*this) - x).LengthSquared(); }
        // Returns the distance between to vectors
        static inline float Distance(const Vector3& a, const Vector3& b)        { return (b - a).Length(); }
        // Returns the squared distance between to vectors
        static inline float DistanceSquared(const Vector3& a, const Vector3& b) { return (b - a).LengthSquared(); }

        // Floor
        void Floor()
        {
            x = floor(x);
            y = floor(y);
            z = floor(z);
        }

        // Return absolute vector
        [[nodiscard]] Vector3 Abs() const { return Vector3(Math::Helper::Abs(x), Math::Helper::Abs(y), Math::Helper::Abs(z)); }

        // Linear interpolation with another vector.
        Vector3 Lerp(const Vector3& v, float t)                                    const { return *this * (1.0f - t) + v * t; }
        static inline Vector3 Lerp(const Vector3& a, const Vector3& b, const float t)    { return a + (b - a) * t; }

        Vector3 operator*(const Vector3& b) const
        {
            return Vector3(
                x * b.x,
                y * b.y,
                z * b.z
            );
        }

        void operator*=(const Vector3& b)
        {
            x *= b.x;
            y *= b.y;
            z *= b.z;
        }

        Vector3 operator*(const float value) const
        {
            return Vector3(
                x * value,
                y * value,
                z * value
            );
        }

        void operator*=(const float value)
        {
            x *= value;
            y *= value;
            z *= value;
        }

        Vector3 operator+(const Vector3& b) const   { return Vector3(x + b.x, y + b.y, z + b.z); }
        Vector3 operator+(const float value) const  { return Vector3(x + value, y + value, z + value); }

        void operator+=(const Vector3& b)
        {
            x += b.x;
            y += b.y;
            z += b.z;
        }

        void operator+=(const float value)
        {
            x += value;
            y += value;
            z += value;
        }

        Vector3 operator-(const Vector3& b) const   { return Vector3(x - b.x, y - b.y, z - b.z); }
        Vector3 operator-(const float value) const  { return Vector3(x - value, y - value, z - value); }

        void operator-=(const Vector3& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
        }

        Vector3 operator/(const Vector3& rhs) const { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
        Vector3 operator/(const float rhs) const    { return Vector3(x / rhs, y / rhs, z / rhs); }

        void operator/=(const Vector3& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
        }

        // Test for equality without using epsilon
        bool operator==(const Vector3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        // Test for inequality without using epsilon
        bool operator!=(const Vector3& rhs) const
        {
            return !(*this == rhs);
        }

        // Return negation.
        Vector3 operator -() const { return Vector3(-x, -y, -z); }

        [[nodiscard]] std::string ToString() const;
        [[nodiscard]] const float* Data() const { return &x; }

        float x;
        float y;
        float z;

        static const Vector3 Zero;
        static const Vector3 Left;
        static const Vector3 Right;
        static const Vector3 Up;
        static const Vector3 Down;
        static const Vector3 Forward;
        static const Vector3 Backward;
        static const Vector3 One;
        static const Vector3 Infinity;
        static const Vector3 InfinityNeg;
    };

    // Reverse order operators
    inline LC_CLASS Vector3 operator*(float lhs, const Vector3& rhs) { return rhs * lhs; }
}
