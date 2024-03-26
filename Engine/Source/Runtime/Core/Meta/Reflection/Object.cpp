
#include "Runtime/Core/pch.h"
#include <cstdint>

#include "Runtime/Core/Meta/Reflection/object.h"

namespace LitchiRuntime
{
	uint64_t g_id = 0;

	Object::Object()
	{
		m_object_id = GenerateObjectId();
	}
}
