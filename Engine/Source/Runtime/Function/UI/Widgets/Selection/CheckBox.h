
#pragma once

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/UI/Widgets/DataWidget.h"

namespace LitchiRuntime
{
	/**
	* Checkbox widget that can be checked or not
	*/
	class CheckBox : public LitchiRuntime::DataWidget<bool>
	{
	public:
		/** 
		* Constructor
		* @param p_value
		* @param p_label
		*/
		CheckBox(bool p_value = false, const std::string& p_label = "");

	protected:
		void _Draw_Impl() override;

	public:
		bool value;
		std::string label;
		LitchiRuntime::Event<bool> ValueChangedEvent;
	};
}
