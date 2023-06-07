
#pragma once

#include "Light.h"

namespace LitchiRuntime
{
	/**
	* A light that is localized, has attenuation and has a direction
	*/
	class SpotLight : public LightComponent
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		SpotLight();
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
		* Returns the light cutoff
		*/
		float GetCutoff() const;

		/**
		* Returns the light outercutoff
		*/
		float GetOuterCutoff() const;

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

		/**
		* Defines the light cutoff
		* @param p_cutoff
		*/
		void SetCutoff(float p_cutoff);

		/**
		* Defines the light outercutoff
		* @param p_cutoff
		*/
		void SetOuterCutoff(float p_outerCutoff);
	};
}