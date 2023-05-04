
#pragma once

#include "core/type.hpp"
#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Simple widget that display a progress bar
	*/
	class ProgressBar : public LitchiRuntime::AWidget
	{
	public:
		/**
		* Constructor
		* @param p_fraction
		* @param p_size
		* @param p_overlay
		*/
		ProgressBar(float p_fraction = 0.0f, const glm::vec2& p_size = glm::vec2( 0.0f, 0.0f ), const std::string& p_overlay = "");

	protected:
		void _Draw_Impl() override;

	public:
		float fraction;
		glm::vec2 size;
		std::string overlay;
	};
}
