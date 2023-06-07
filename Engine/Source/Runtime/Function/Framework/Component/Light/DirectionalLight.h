
#pragma once

#include "Light.h"

namespace LitchiRuntime
{
	/**
	* A simple light that has no attenuation and that has a direction
	*/
	class DirectionalLight : public LightComponent
	{
	public:
		/**
		* Constructor
		*/
		DirectionalLight();
	public:

		RTTR_ENABLE(LightComponent)
	};
}