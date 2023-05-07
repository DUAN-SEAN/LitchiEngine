
#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "core/type.hpp"
#include "gtc/quaternion.hpp"

namespace LitchiRuntime
{
	namespace Math
	{
		const glm::vec3 One(1.0f, 1.0f, 1.0f);
		const glm::vec3 Zero(0.0f, 0.0f, 0.0f);
		const glm::vec3 Forward(0.0f, 0.0f, 1.0f);
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
