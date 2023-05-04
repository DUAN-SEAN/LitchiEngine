
#pragma once

#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Simple widget that display a separator
	*/
	class Separator : public LitchiRuntime::AWidget
	{
	protected:
		void _Draw_Impl() override;
	};
}
