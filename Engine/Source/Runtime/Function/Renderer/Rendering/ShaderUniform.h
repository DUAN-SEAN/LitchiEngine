
#pragma once

#include <string>
#include <any>

#include "UniformType.h"

namespace LitchiRuntime
{
	/**
	* Data structure containing information about a uniform
	*/
	struct ShaderUniform
	{
		UniformType		type;
		std::string		name;
		uint32_t		location;
		int size;
		std::vector<ShaderUniform>* memberUniform;
	};
}
