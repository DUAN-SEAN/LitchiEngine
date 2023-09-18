
#pragma once

#include "rttr/registration"
#include "Runtime/Core/Definitions.h"

namespace LitchiRuntime
{
	class Object
	{
	public:
		Object(){}
		~Object() {}

		/**
		 * \brief 当资源修改后调用
		 */
		virtual void PostResourceModify(){}

		/**
		 * \brief 当资源加载完成后调用
		 */
		virtual void PostResourceLoaded(){}

		RTTR_ENABLE()
	};
}
