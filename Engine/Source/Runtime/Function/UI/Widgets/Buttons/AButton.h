
#pragma once

#include <string>

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Base class for any button widget
	*/
	class AButton : public AWidget
	{
	protected:
		void _Draw_Impl() override = 0;

	public:
		Event<> ClickedEvent;
	};
}
