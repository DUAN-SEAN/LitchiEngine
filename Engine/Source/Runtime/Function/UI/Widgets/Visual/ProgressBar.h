
#pragma once


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
		ProgressBar(float p_fraction = 0.0f, const Vector2& p_size = Vector2( 0.0f, 0.0f ), const std::string& p_overlay = "");

	protected:
		void _Draw_Impl() override;

	public:
		float fraction;
		Vector2 size;
		std::string overlay;
	};
}
