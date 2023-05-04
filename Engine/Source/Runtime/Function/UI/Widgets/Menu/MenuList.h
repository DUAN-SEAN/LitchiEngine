
#pragma once

#include <vector>

#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"

namespace LitchiRuntime
{
	/**
	* Widget that behave like a group with a menu display
	*/
	class MenuList : public Group
	{
	public:
		/**
		* Constructor
		* @param p_name
		* @param p_locked
		*/
		MenuList(const std::string& p_name, bool p_locked = false);

	protected:
		virtual void _Draw_Impl() override;

	public:
		std::string name;
		bool locked;
		Event<> ClickedEvent;

	private:
		bool m_opened;
	};
}
