
#pragma once

#include "Text.h"
#include "Runtime/Core/Tools/Eventing/Event.h"

namespace LitchiRuntime
{
	/**
	* Widget to display text on a panel that is also clickable
	*/
	class TextClickable : public Text
	{
	public:
		/**
		* Constructor
		* @param p_content
		*/
		TextClickable(const std::string& p_content = "");

	protected:
		virtual void _Draw_Impl() override;

	public:
		Event<> ClickedEvent;
		Event<> DoubleClickedEvent;
	};
}
