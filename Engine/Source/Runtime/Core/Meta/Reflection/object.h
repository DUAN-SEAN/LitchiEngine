
#pragma once

#include "rttr/registration"
#include "Runtime/Core/Definitions.h"

namespace LitchiRuntime
{
	// Globals
	extern uint64_t g_id;

	class Object
	{
	public:
		Object();
		~Object() {}

		// Name
		std::string& GetObjectName()    { return m_object_name; }
		void SetObjectName(std::string& name) { m_object_name = name; }

		// Id
		uint64_t GetObjectId()        const { return m_object_id; }
		void SetObjectId(uint64_t id) { m_object_id = id; }
		static uint64_t GenerateObjectId() { return ++g_id; }

		// CPU & GPU sizes
		const uint64_t GetObjectSizeCpu() const { return m_object_size_cpu; }
		const uint64_t GetObjectSizeGpu() const { return m_object_size_gpu; }

		/**
		 * \brief 当资源修改后调用
		 */
		virtual void PostResourceModify() {}

		/**
		 * \brief 当资源加载完成后调用
		 */
		virtual void PostResourceLoaded() {}

		RTTR_ENABLE()

	protected:
		std::string m_object_name;
		uint64_t m_object_id = 0;
		uint64_t m_object_size_cpu = 0;
		uint64_t m_object_size_gpu = 0;
	};
}
