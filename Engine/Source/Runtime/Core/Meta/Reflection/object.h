
#pragma once

#include "rttr/registration"

namespace LitchiRuntime
{
	class Object
	{
	public:
		Object(){}
		~Object() {}

		/**
		 * \brief 当资源加载完成后调用
		 */
		virtual void PostResourceLoaded(){}

		RTTR_ENABLE()
	};
}
