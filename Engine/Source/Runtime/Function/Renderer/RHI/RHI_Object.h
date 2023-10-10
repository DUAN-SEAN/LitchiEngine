
#pragma once

#include "Runtime/Core/Meta/Reflection/object.h"

namespace LitchiRuntime
{
	// Globals
	extern uint64_t g_id;

	class RHI_Object :public LitchiRuntime::Object
	{
	public:
		RHI_Object();

		// Name
		const std::string& GetObjectName()    const { return m_object_name; }
		void SetObjectName(const std::string& name) { m_object_name = name; }

		// Id
		const uint64_t GetObjectId()        const { return m_object_id; }
		void SetObjectId(const uint64_t id) { m_object_id = id; }
		static uint64_t GenerateObjectId() { return ++g_id; }

		// CPU & GPU sizes
		const uint64_t GetObjectSizeCpu() const { return m_object_size_cpu; }
		const uint64_t GetObjectSizeGpu() const { return m_object_size_gpu; }

	protected:
		std::string m_object_name;
		uint64_t m_object_id = 0;
		uint64_t m_object_size_cpu = 0;
		uint64_t m_object_size_gpu = 0;
	};
}
