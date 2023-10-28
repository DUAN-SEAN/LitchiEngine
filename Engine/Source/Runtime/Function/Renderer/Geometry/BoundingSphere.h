
#pragma once

#include "Runtime/Core/Math/Vector3.h"

namespace LitchiRuntime
{
	/**
	* Data structure that defines a bounding sphere (Position + radius)
	*/
	struct BoundingSphere
	{
		Vector3 position;
		float radius;
	};
}
