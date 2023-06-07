
#pragma once

#include "Light.h"

namespace LitchiRuntime
{
	/**
	* A light that is localized and has attenuation
	*/
	class PointLight : public LightComponent
	{
	public:
		PointLight();
		/**
		* Returns the light constant
		*/
		float GetConstant() const;

		/**
		* Returns the light linear
		*/
		float GetLinear() const;

		/**
		* Returns the light quadratic
		*/
		float GetQuadratic() const;

		/**
		* Defines the light constant
		* @param p_constant
		*/
		void SetConstant(float p_constant);

		/**
		* Defines the light linear
		* @param p_linear
		*/
		void SetLinear(float p_linear);

		/**
		* Defines the light quadratic
		* @param p_quadratic
		*/
		void SetQuadratic(float p_quadratic);

	public:

		RTTR_ENABLE(LightComponent)
	};
}