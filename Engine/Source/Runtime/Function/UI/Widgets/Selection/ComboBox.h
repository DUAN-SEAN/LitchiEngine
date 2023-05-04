
#pragma once

#include <map>

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/UI/Widgets/DataWidget.h"

namespace LitchiRuntime
{
	/**
	* Widget that can display a list of values that the user can select
	*/
	class ComboBox : public DataWidget<int>
	{
	public:
		/**
		* Constructor
		* @param p_currentChoice
		*/
		ComboBox(int p_currentChoice = 0);

	protected:
		void _Draw_Impl() override;

	public:
		std::map<int, std::string> choices;
		int currentChoice;

	public:
		Event<int> ValueChangedEvent;
	};
}
