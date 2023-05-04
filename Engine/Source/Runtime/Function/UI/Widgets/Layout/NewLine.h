
#pragma once

#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Widget that adds an empty line to the panel
	*/
	class NewLine : public AWidget
	{
	protected:
		void _Draw_Impl() override;
	};
}
