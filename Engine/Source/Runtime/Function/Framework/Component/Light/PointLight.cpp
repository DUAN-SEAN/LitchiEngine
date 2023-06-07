
#include "PointLight.h"

LitchiRuntime::PointLight::PointLight() :
	LightComponent()
{
	m_data.type = static_cast<float>(Light::Type::POINT);
}

float LitchiRuntime::PointLight::GetConstant() const
{
	return m_data.constant;
}

float LitchiRuntime::PointLight::GetLinear() const
{
	return m_data.linear;
}

float LitchiRuntime::PointLight::GetQuadratic() const
{
	return m_data.quadratic;
}

void LitchiRuntime::PointLight::SetConstant(float p_constant)
{
	m_data.constant = p_constant;
}

void LitchiRuntime::PointLight::SetLinear(float p_linear)
{
	m_data.linear = p_linear;
}

void LitchiRuntime::PointLight::SetQuadratic(float p_quadratic)
{
	m_data.quadratic = p_quadratic;
}
