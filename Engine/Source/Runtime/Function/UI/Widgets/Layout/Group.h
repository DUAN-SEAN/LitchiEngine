
#pragma once

#include <vector>

#include "Runtime/Function/UI/Internal/WidgetContainer.h"
#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Widget that can contains other widgets
	*/
	class Group : public AWidget, public WidgetContainer
	{
	protected:
		virtual void _Draw_Impl() override;
	};
}
