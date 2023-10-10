
#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include "glm.hpp"
#include "gtc/quaternion.hpp"

namespace LitchiRuntime
{
	namespace Math
	{
		const glm::vec3 One(1.0f, 1.0f, 1.0f);
		const glm::vec3 Zero(0.0f, 0.0f, 0.0f);
		const glm::vec3 Forward(0.0f, 0.0f, -1.0f);
		const glm::vec3 Right(1.0f, 0.0f, 0.0f);
		const glm::vec3 Up(0.0f, 1.0f, 0.0f);

		const glm::quat Identity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		glm::quat Lerp(glm::quat& p_start, glm::quat& p_end, float p_alpha)
		{
			p_alpha = std::clamp(p_alpha, 0.f, 1.f);

			glm::quat q;
			
			if (glm::dot(p_start, p_end) < 0.f)
			{
				q.x = p_start.x + p_alpha * (-p_end.x - p_start.x);
				q.y = p_start.y + p_alpha * (-p_end.y - p_start.y);
				q.z = p_start.z + p_alpha * (-p_end.z - p_start.z);
				q.w = p_start.w + p_alpha * (-p_end.w - p_start.w);
			}
			else
			{
				q.x = p_start.x + (p_end.x - p_start.x) * p_alpha;
				q.y = p_start.y + (p_end.y - p_start.y) * p_alpha;
				q.z = p_start.z + (p_end.z - p_start.z) * p_alpha;
				q.w = p_start.w + (p_end.w - p_start.w) * p_alpha;
			}

			return glm::normalize(q);
		}

		glm::quat LookAt(const glm::vec3& p_forward, const glm::vec3& p_up)
		{
			auto vector = glm::normalize(p_forward);
			auto vector2 = glm::normalize(glm::cross(p_up, vector));
			auto vector3 = glm::cross(vector, vector2);
			auto m00 = vector2.x;
			auto m01 = vector2.y;
			auto m02 = vector2.z;
			auto m10 = vector3.x;
			auto m11 = vector3.y;
			auto m12 = vector3.z;
			auto m20 = vector.x;
			auto m21 = vector.y;
			auto m22 = vector.z;


			float num8 = (m00 + m11) + m22;
			auto quaternion = Identity;
			if (num8 > 0.f)
			{
				auto num = sqrt(num8 + 1.f);
				quaternion.w = num * 0.5f;
				num = 0.5f / num;
				quaternion.x = (m12 - m21) * num;
				quaternion.y = (m20 - m02) * num;
				quaternion.z = (m01 - m10) * num;
				return quaternion;
			}
			if ((m00 >= m11) && (m00 >= m22))
			{
				auto num7 = sqrt(((1.f + m00) - m11) - m22);
				auto num4 = 0.5f / num7;
				quaternion.x = 0.5f * num7;
				quaternion.y = (m01 + m10) * num4;
				quaternion.z = (m02 + m20) * num4;
				quaternion.w = (m12 - m21) * num4;
				return quaternion;
			}
			if (m11 > m22)
			{
				auto num6 = sqrt(((1.f + m11) - m00) - m22);
				auto num3 = 0.5f / num6;
				quaternion.x = (m10 + m01) * num3;
				quaternion.y = 0.5f * num6;
				quaternion.z = (m21 + m12) * num3;
				quaternion.w = (m20 - m02) * num3;
				return quaternion;
			}
			auto num5 = sqrt(((1.f + m22) - m00) - m11);
			auto num2 = 0.5f / num5;
			quaternion.x = (m20 + m02) * num2;
			quaternion.y = (m21 + m12) * num2;
			quaternion.z = 0.5f * num5;
			quaternion.w = (m01 - m10) * num2;
			return quaternion;
		}
	}

}



namespace LitchiRuntime
{
    enum class Intersection
    {
        Outside,
        Inside,
        Intersects
    };
}

namespace LitchiRuntime::Math::Helper
{
    constexpr float EPSILON = std::numeric_limits<float>::epsilon();
    constexpr float INFINITY_ = std::numeric_limits<float>::infinity();
    constexpr float PI = 3.14159265359f;
    constexpr float PI_2 = 6.28318530718f;
    constexpr float PI_4 = 12.5663706144f;
    constexpr float PI_DIV_2 = 1.57079632679f;
    constexpr float PI_DIV_4 = 0.78539816339f;
    constexpr float PI_INV = 0.31830988618f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;

    inline double Cot(float x) { return cos(x) / sin(x); }
    inline float CotF(float x) { return cosf(x) / sinf(x); }
    constexpr float DegreesToRadians(const float degrees) { return degrees * DEG_TO_RAD; }
    constexpr float RadiansToDegrees(const float radians) { return radians * RAD_TO_DEG; }

    template <typename T>
    constexpr T Clamp(T x, T a, T b) { return x < a ? a : (x > b ? b : x); }

    template <typename T>
    constexpr T Saturate(T x) { return Clamp<T>(x, static_cast<T>(0), static_cast<T>(1)); }

    // Lerp linearly between to values
    template <class T, class U>
    constexpr T Lerp(T lhs, T rhs, U t) { return lhs * (static_cast<U>(1) - t) + rhs * t; }

    // Returns the absolute value
    template <class T>
    constexpr T Abs(T value) { return value >= static_cast<T>(0) ? value : -value; }

    // Check for equality but allow for a small error
    template <class T>
    constexpr bool Equals(T lhs, T rhs, T error = std::numeric_limits<T>::epsilon()) { return lhs + error >= rhs && lhs - error <= rhs; }

    template <class T>
    constexpr T Max(T a, T b) { return a > b ? a : b; }

    template <class T>
    constexpr T Max3(T a, T b, T c) { return Max(a, Max(b, c)); }

    template <class T>
    constexpr T Min(T a, T b) { return a < b ? a : b; }

    template <class T>
    constexpr T Min3(T a, T b, T c) { return Min(a, Min(b, c)); }

    template <class T>
    constexpr T Sqrt(T x) { return sqrt(x); }

    template <class T>
    constexpr T Floor(T x) { return floor(x); }

    template <class T>
    constexpr T Ceil(T x) { return ceil(x); }

    template <class T>
    constexpr T Round(T x) { return round(x); }

    template <class T>
    constexpr T Tan(T x) { return tan(x); }

    template <class T>
    constexpr T Cos(T x) { return cos(x); }

    template <class T>
    constexpr T Sin(T x) { return sin(x); }

    template <typename T>
    constexpr int Sign(T x) { return (static_cast<T>(0) < x) - (x < static_cast<T>(0)); }

    template <class T>
    constexpr T Pow(T x, T y) { return pow(x, y); }

    template <class T>
    constexpr T Log(T x) { return log(x); }

    template <class T>
    inline T Random(T from = static_cast<T>(0), T to = static_cast<T>(1))
    {
        std::random_device rd;                              // obtain a random number from hardware
        std::mt19937 eng(rd());                             // seed the generator
        std::uniform_real_distribution<T> distr(from, to);  // define the range
        return distr(eng);
    }

    constexpr uint32_t PreviousPowerOfTwo(uint32_t x)
    {
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        return x - (x >> 1);
    }

    constexpr uint32_t NextPowerOfTwo(uint32_t x)
    {
        if (x < 2)
            return 2;

        if (x % 2 == 0)
            return x << 1;

        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x++;
    }
}
