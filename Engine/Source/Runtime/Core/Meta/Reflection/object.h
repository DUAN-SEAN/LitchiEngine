
#pragma once

#include "rttr/registration"
#include "Runtime/Core/Definitions.h"

namespace LitchiRuntime
{
	/**
	 * @brief Globals Generate Id
	*/
	extern uint64_t g_id;

	/**
	 * @brief Engine Base Object
	 * All engine object derived this
	*/
	class Object
	{
	public:

		/**
		 * @brief Construct
		*/
		Object();

		virtual ~Object() = default;

		/**
		 * @brief Get object name
		 * @return 
		*/
		std::string& GetObjectName()    { return m_object_name; }

		/**
		 * @brief Set object name
		 * @param name 
		*/
		void SetObjectName(std::string& name) { m_object_name = name; }

		/**
		 * @brief Get runtime generated id
		 * @return 
		*/
		uint64_t GetObjectId()        const { return m_object_id; }

		/**
		 * @brief Set runtime generated id
		 * @param id 
		*/
		void SetObjectId(uint64_t id) { m_object_id = id; }

		/**
		 * @brief Generate a runtime id
		 * @return 
		*/
		static uint64_t GenerateObjectId() { return ++g_id; }

		/**
		 * @brief Get object cpu size
		 * @return 
		*/
		const uint64_t GetObjectSizeCpu() const { return m_object_size_cpu; }

		/**
		 * @brief Get object gpu size
		 * @return 
		*/
		const uint64_t GetObjectSizeGpu() const { return m_object_size_gpu; }

		/**
		 * @brief Call before object resource change
		*/
		virtual void PostResourceModify() {}

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		virtual void PostResourceLoaded() {}

		RTTR_ENABLE()

	protected:

		/**
		 * @brief Object name
		*/
		std::string m_object_name;

		/**
		 * @brief Object runtime id
		*/
		uint64_t m_object_id = 0;

		/**
		 * @brief Object cpu size
		*/
		uint64_t m_object_size_cpu = 0;

		/**
		 * @brief Object gpu size
		*/
		uint64_t m_object_size_gpu = 0;
	};
}
