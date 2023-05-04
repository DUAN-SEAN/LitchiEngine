
#pragma once

#include "Runtime/Function/UI/Plugins/IPlugin.h"
#include "Runtime/Function/UI/Internal/WidgetContainer.h"
#include "Runtime/Function/UI/Widgets/Menu/MenuList.h"
#include "Runtime/Function/UI/Widgets/Menu/MenuItem.h"

namespace LitchiRuntime
{
	/**
	* A simple plugin that will show a contextual menu on right click
	* You can add widgets to a contextual menu
	*/
	class ContextualMenu : public IPlugin, public WidgetContainer
	{
	public:
		/**
		* Execute the plugin
		*/
		void Execute();

		/**
		* Force close the contextual menu
		*/
		void Close();
	};
}
