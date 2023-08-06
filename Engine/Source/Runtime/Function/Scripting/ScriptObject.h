
#pragma once

#include "Runtime/Core/Meta/Reflection/object.h"

namespace LitchiRuntime
{
	class ScriptObject : Object
	{
	public:
		uint64_t GetUnmanagedId()
		{
			return m_unmanagedId;
		}

	private:
		uint64_t m_unmanagedId;
	};
}
