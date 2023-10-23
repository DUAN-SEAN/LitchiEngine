
#pragma once

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/Renderer/Rendering/Color.h"
#include "Runtime/Function/UI/Widgets/DataWidget.h"

namespace LitchiRuntime
{
	/**
	* Widget that can open a color picker on click
	*/
	class ColorEdit : public LitchiRuntime::DataWidget<LitchiRuntime::Color>
	{
	public:
		/**
		* Constructor
		* @param p_enableAlpha
		* @param p_defaultColor
		*/
		ColorEdit(bool p_enableAlpha = false, const LitchiRuntime::Color& p_defaultColor = {1.0f,1.0f ,1.0f ,1.0f });

	protected:
		void _Draw_Impl() override;

	public:
		bool enableAlpha;
		LitchiRuntime::Color color;
		LitchiRuntime::Event<LitchiRuntime::Color&> ColorChangedEvent;
	};
}
