
#pragma once

#include <cmath>
#include <limits>
#include <random>

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

	/**
	 * @brief Lerp linearly between to values
	 * @tparam T 
	 * @tparam U 
	 * @param lhs 
	 * @param rhs 
	 * @param t 
	 * @return 
	*/
	template <class T, class U>
	constexpr T Lerp(T lhs, T rhs, U t) { return lhs * (static_cast<U>(1) - t) + rhs * t; }

	/**
	 * @brief Returns the absolute value
	 * @tparam T 
	 * @param value 
	 * @return 
	*/
	template <class T>
	constexpr T Abs(T value) { return value >= static_cast<T>(0) ? value : -value; }

	/**
	 * @brief  Check for equality but allow for a small error
	 * @tparam T 
	 * @param lhs 
	 * @param rhs 
	 * @param error 
	 * @return 
	*/
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
