
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
		UniformType		type{UniformType::UNIFORM_Unknown };
		std::string		name{};
		uint32_t		location{0};
		int size{0};
		std::vector<ShaderUniform>* memberUniform{nullptr};
	};
}
