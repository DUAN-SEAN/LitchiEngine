
#pragma once

#include "Runtime/Core/Meta/Reflection/object.h"

#include "rttr/registration"

namespace LitchiRuntime
{
	class ScriptObject : public Object
	{
	public:
		uint64_t GetUnmanagedId()
		{
			return m_unmanagedId;
		}
	public:

		RTTR_ENABLE(Object)

	protected:

		uint64_t m_unmanagedId;
	};
}
