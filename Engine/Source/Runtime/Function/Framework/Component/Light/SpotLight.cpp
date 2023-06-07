
#include "SpotLight.h"

LitchiRuntime::SpotLight::SpotLight() :
	LightComponent()
{
	m_data.type = static_cast<float>(Light::Type::SPOT);
}

float LitchiRuntime::SpotLight::GetConstant() const
{
	return m_data.constant;
}

float LitchiRuntime::SpotLight::GetLinear() const
{
	return m_data.linear;
}

float LitchiRuntime::SpotLight::GetQuadratic() const
{
	return m_data.quadratic;
}

float LitchiRuntime::SpotLight::GetCutoff() const
{
	return m_data.cutoff;
}

float LitchiRuntime::SpotLight::GetOuterCutoff() const
{
	return m_data.outerCutoff;
}

void LitchiRuntime::SpotLight::SetConstant(float p_constant)
{
	m_data.constant = p_constant;
}

void LitchiRuntime::SpotLight::SetLinear(float p_linear)
{
	m_data.linear = p_linear;
}

void LitchiRuntime::SpotLight::SetQuadratic(float p_quadratic)
{
	m_data.quadratic = p_quadratic;
}

void LitchiRuntime::SpotLight::SetCutoff(float p_cutoff)
{
	m_data.cutoff = p_cutoff;
}

void LitchiRuntime::SpotLight::SetOuterCutoff(float p_outerCutoff)
{
	m_data.outerCutoff = p_outerCutoff;
}