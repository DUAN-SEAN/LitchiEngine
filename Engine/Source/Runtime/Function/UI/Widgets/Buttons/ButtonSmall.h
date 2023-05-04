
#pragma once

#include <string>

#include "AButton.h"
#include "Runtime/Function/UI/Types/Color.h"

namespace LitchiRuntime
{
	/**
	* Small button widget
	*/
	class ButtonSmall : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_label
		*/
		ButtonSmall(const std::string& p_label = "");

	protected:
		void _Draw_Impl() override;

	public:
		std::string label;

		Color idleBackgroundColor;
		Color hoveredBackgroundColor;
		Color clickedBackgroundColor;

		Color textColor;
	};
}
