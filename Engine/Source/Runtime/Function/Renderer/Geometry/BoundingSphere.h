
#pragma once

#include "core/type.hpp"

namespace LitchiRuntime
{
	/**
	* Data structure that defines a bounding sphere (Position + radius)
	*/
	struct BoundingSphere
	{
		glm::vec3 position;
		float radius;
	};
}
