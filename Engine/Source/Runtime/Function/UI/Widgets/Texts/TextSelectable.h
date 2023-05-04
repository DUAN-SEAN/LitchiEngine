
#pragma once
#include "Text.h"
#include "Runtime/Core/Tools/Eventing/Event.h"

namespace LitchiRuntime
{
	/**
	* Simple widget to display a selectable text on a panel
	*/
	class TextSelectable : public Text
	{
	public:
		/**
		* Constructor
		* @param p_content
		* @param p_selected
		* @param p_disabled
		*/
		TextSelectable(const std::string& p_content = "", bool p_selected = false, bool p_disabled = false);

	protected:
		virtual void _Draw_Impl() override;

	public:
		bool selected;
		bool disabled;

		Event<bool> ClickedEvent;
		Event<> SelectedEvent;
		Event<> UnselectedEvent;
	};
}
