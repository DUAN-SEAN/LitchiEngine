
#pragma once

#include <string>

#include "AButton.h"
#include "glm.hpp"
#include "Runtime/Function/UI/Types/Color.h"

namespace LitchiRuntime
{
	/**
	* Simple button widget
	*/
	class Button : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_label
		* @param p_size
		* @param p_disabled
		*/
		Button(const std::string& p_label = "", const glm::vec2& p_size = glm::vec2(0.f, 0.f), bool p_disabled = false);

	protected:
		void _Draw_Impl() override;

	public:
		std::string label;
		glm::vec2 size;
		bool disabled = false;

		Color idleBackgroundColor;
		Color hoveredBackgroundColor;
		Color clickedBackgroundColor;

		Color textColor;
	};
}
