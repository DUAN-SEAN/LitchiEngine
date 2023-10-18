
#pragma once

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/UI/Types/Color.h"
#include "Runtime/Function/UI/Widgets/DataWidget.h"

namespace LitchiRuntime
{
	/**
	* Widget that allow the selection of a color with a color picker
	*/
	class ColorPicker : public DataWidget<Color>
	{
	public:
		/**
		* Constructor
		* @param p_enableAlpha
		* @param p_defaultColor
		*/
		ColorPicker(bool p_enableAlpha = false, const Color& p_defaultColor = { 1.0f,1.0f ,1.0f ,1.0f });

	protected:
		void _Draw_Impl() override;

	public:
		bool enableAlpha;
		Color color;
		Event<Color&> ColorChangedEvent;
	};
}
