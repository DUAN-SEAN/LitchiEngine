/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/UI/Widgets/DataWidget.h"

namespace LitchiRuntime
{
	/**
	* Input widget of type string
	*/
	class InputText : public DataWidget<std::string>
	{
	public:
		/**
		* Constructor
		* @param p_content
		* @param p_label
		*/
		InputText(const std::string& p_content = "", const std::string& p_label = "");

	protected:
		void _Draw_Impl() override;

	public:
		std::string content;
		std::string label;
		bool selectAllOnClick = false;
		Event<std::string> ContentChangedEvent;
		Event<std::string> EnterPressedEvent;
	};
}
