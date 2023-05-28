
#pragma once

#include "InputSingleScalar.h"

namespace LitchiRuntime
{
	/**
	* Input widget of type int32
	*/
	class InputInt32 : public InputSingleScalar<int32_t>
	{
	public:
		/**
		* Constructor
		* @param p_defaultValue
		* @param p_step
		* @param p_fastStep
		* @param p_label
		* @param p_format
		* @param p_selectAllOnClick
		*/
		InputInt32
		(
			int32_t p_defaultValue = 0,
			int32_t p_step = 1,
			int32_t p_fastStep = 0,
			const std::string& p_label = "",
			const std::string& p_format = "",
			bool p_selectAllOnClick = true
		);
	};
}
