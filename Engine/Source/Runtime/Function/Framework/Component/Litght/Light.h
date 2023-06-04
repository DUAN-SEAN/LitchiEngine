
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Light/Light.h"

namespace LitchiRuntime
{
	/**
	* Base class for any light
	*/
	class LightComponent : public Component
	{
	public:
		/**
		* Constructor
		*/
		LightComponent();

		/**
		* Returns light data
		*/
		const Light& GetData() const;

		/**
		* Returns light color
		*/
		const glm::vec3& GetColor() const;

		/**
		* Returns light intensity
		*/
		float GetIntensity() const;

		/**
		* Defines a new color for the light
		* @param p_color
		*/
		void SetColor(const glm::vec3& p_color);

		/**
		* Defines the intensity for the light
		* @param p_intensity
		*/
		void SetIntensity(float p_intensity);

	public:

		void SetLight(Light light)
		{
			m_data = light;
		}
		Light GetLight() { return m_data; }

		RTTR_ENABLE(Component)

	protected:
		Light m_data;
	};
}
