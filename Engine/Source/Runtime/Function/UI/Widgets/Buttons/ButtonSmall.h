
#pragma once

#include <string>

#include "AButton.h"
#include "Runtime/Function/Renderer/Rendering/Color.h"

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

		Color idleBackgroundColor{ 1.0f,1.0f ,1.0f ,1.0f };
		Color hoveredBackgroundColor{ 1.0f,1.0f ,1.0f ,1.0f };
		Color clickedBackgroundColor{ 1.0f,1.0f ,1.0f ,1.0f };

		Color textColor{ 1.0f,1.0f ,1.0f ,1.0f };
	};
}
