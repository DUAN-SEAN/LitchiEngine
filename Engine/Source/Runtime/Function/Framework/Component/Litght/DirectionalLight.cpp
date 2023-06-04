
#include "DirectionalLight.h"

LitchiRuntime::DirectionalLight::DirectionalLight():
	LightComponent()
{
	m_data.type = static_cast<float>(Light::Type::DIRECTIONAL);
}
