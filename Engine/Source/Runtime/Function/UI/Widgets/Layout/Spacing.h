
#pragma once

#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Widget that adds a space to the panel line
	*/
	class Spacing : public AWidget
	{
	public:
		/**
		* Constructor
		* @param p_spaces
		*/
		Spacing(uint16_t p_spaces = 1);

	protected:
		void _Draw_Impl() override;

	public:
		uint16_t spaces = 1;
	};
}
