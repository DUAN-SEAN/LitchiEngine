
#pragma once

#include <stdint.h>

namespace LitchiRuntime
{
	/**
	* Defines the types that a uniform can take
	*/
	enum class UniformType : uint32_t
	{
		UNIFORM_Unknown = 0x00,
		UNIFORM_BOOL,
		UNIFORM_INT,
		UNIFORM_UINT,
		UNIFORM_FLOAT,
		UNIFORM_FLOAT_VEC2,
		UNIFORM_FLOAT_VEC3,
		UNIFORM_FLOAT_VEC4,
		UNIFORM_FLOAT_MAT4,
		UNIFORM_DOUBLE_MAT4,
		UNIFORM_Texture,
		UNIFORM_Struct,
	};
}