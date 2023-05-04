
#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "APanel.h"
#include "Runtime/Function/UI//Widgets/Menu/MenuList.h"

namespace LitchiRuntime
{
	/**
	* A simple panel that will be displayed on the top side of the canvas
	*/
	class PanelMenuBar : public APanel
	{
	protected:
		void _Draw_Impl() override;
	};
}