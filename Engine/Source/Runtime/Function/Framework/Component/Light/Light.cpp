
#include "Light.h"

LitchiRuntime::LightComponent::LightComponent() :m_data({}), Component()
{

}

const LitchiRuntime::Light& LitchiRuntime::LightComponent::GetData() const
{
	return m_data;
}

const glm::vec3& LitchiRuntime::LightComponent::GetColor() const
{
	return m_data.color;
}

float LitchiRuntime::LightComponent::GetIntensity() const
{
	return m_data.intensity;
}

void LitchiRuntime::LightComponent::SetColor(const glm::vec3& p_color)
{
	m_data.color = p_color;
}

void LitchiRuntime::LightComponent::SetIntensity(float p_intensity)
{
	m_data.intensity = p_intensity;
}
