
#pragma once

#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Simple widget that display a bullet point
	*/
	class Bullet : public AWidget
	{
	protected:
		virtual void _Draw_Impl() override;
	};
}
