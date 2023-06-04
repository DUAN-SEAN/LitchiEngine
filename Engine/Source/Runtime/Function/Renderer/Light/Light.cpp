
#include "Light.h"

uint32_t Pack(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3)
{
	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}

uint32_t Pack(const glm::vec3& p_toPack)
{
	return Pack(static_cast<uint8_t>(p_toPack.x * 255.f), static_cast<uint8_t>(p_toPack.y * 255.f), static_cast<uint8_t>(p_toPack.z * 255.f), 0);
}

LitchiRuntime::Light::Light(FTransform & p_tranform, Type p_type) : m_transform(p_tranform), type(static_cast<float>(p_type))
{
	
}

glm::mat4 LitchiRuntime::Light::GenerateMatrix() const
{
	glm::mat4 result;

	// lightPosition = lightMat[0]
	auto position = m_transform.GetWorldPosition();
	result[0][0] = position.x; // 0
	result[0][1] = position.y;// 1
	result[0][2] = position.z;// 2

	// lightForward = lightMat[1]
	auto forward = m_transform.GetWorldForward();
	result[1][0] = forward.x;// 4
	result[1][1] = forward.y;// 5
	result[1][2] = forward.z;// 6

	result[2][0] = static_cast<float>(Pack(color)); // 8bit rgb 

	result[3][0] = type; // 12 
	result[3][1] = cutoff;// 13 cutOff = cos(radians(lightMat[3][1]));
	result[3][2] = outerCutoff;// 14 cutOff = cos(radians(lightMat[3][1] + lightMat[3][2]));

	result[0][3] = constant; // 3 constant = lightMat[0][3]
	result[1][3] = linear; // 7  linear = lightMat[0][3]
	result[2][3] = quadratic;// 11 intensity = lightMat[3][3];
	result[3][3] = intensity; // 15 intensity = lightMat[3][3];

	// none use [2,1] [2,2]

	return result;
}

float CalculateLuminosity(float p_constant, float p_linear, float p_quadratic, float p_intensity, float p_distance)
{
	auto attenuation = (p_constant + p_linear * p_distance + p_quadratic * (p_distance * p_distance));
	return (1.0f / attenuation) * std::abs(p_intensity);
}

float CalculatePointLightRadius(float p_constant, float p_linear, float p_quadratic, float p_intensity)
{
	constexpr float threshold = 1 / 255.0f;
	constexpr float step = 1.0f;

	float distance = 0.0f;

	#define TRY_GREATER(value)\
	else if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, value) > threshold)\
	{\
		distance = value;\
	}

	#define TRY_LESS(value, newValue)\
	else if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, value) < threshold)\
	{\
		distance = newValue;\
	}

	// Prevents infinite while true. If a light has a bigger radius than 10000 we ignore it and make it infinite
	if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, 1000.0f) > threshold)
	{
		return std::numeric_limits<float>::infinity();
	}
	TRY_LESS(20.0f, 0.0f)
	TRY_GREATER(750.0f)
	TRY_LESS(50.0f, 20.0f + step)
	TRY_LESS(100.0f, 50.0f + step)
	TRY_GREATER(500.0f)
	TRY_GREATER(250.0f)

	while (true)
	{
		if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, distance) < threshold) // If the light has a very low luminosity for the given distance, we consider the current distance as the light radius
		{
			return distance;
		}
		else
		{
			distance += step;
		}
	}
}

float CalculateAmbientBoxLightRadius(const glm::vec3& p_position, const glm::vec3& p_size)
{
	return glm::distance(p_position, p_position + p_size);
}

float LitchiRuntime::Light::GetEffectRange() const
{
	switch (static_cast<LitchiRuntime::Light::Type>(static_cast<int>(type)))
	{
	case Type::POINT:
	case Type::SPOT:			return CalculatePointLightRadius(constant, linear, quadratic, intensity);
	case Type::AMBIENT_BOX:		return CalculateAmbientBoxLightRadius(m_transform.GetWorldPosition(), { constant, linear, quadratic });
	case Type::AMBIENT_SPHERE:	return constant;
	}

	return std::numeric_limits<float>::infinity();
}

const LitchiRuntime::FTransform& LitchiRuntime::Light::GetTransform() const
{
	return m_transform;
}
