
#pragma once

#include <vector>

#include "Group.h"
#include "Runtime/Core/Tools/Eventing/Event.h"

namespace LitchiRuntime
{
	/**
	* Widget that can contains other widgets and is collapsable
	*/
	class GroupCollapsable : public Group
	{
	public:
		/**
		* Constructor
		* @param p_name
		* @param p_width
		*/
		GroupCollapsable(const std::string& p_name = "",const float p_width = 0.0f);

	protected:
		virtual void _Draw_Impl() override;

	public:
		std::string name;
		float width =0.0f;
		bool closable = false;
		bool opened = true;
		Event<> CloseEvent;
		Event<> OpenEvent;
	};
}
