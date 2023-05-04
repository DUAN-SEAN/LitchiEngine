
#pragma once

#include <stdint.h>

namespace LitchiRuntime
{
	/**
	* Simple union necessary for imgui textureID
	*/
	union TextureID
	{
		uint32_t id;
		void* raw;
	};
}