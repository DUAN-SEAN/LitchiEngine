
#pragma once

#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Overlap Start In Window
	*/
	class OverlapStart : public LitchiRuntime::AWidget
	{
	public:
		OverlapStart() = default;
		OverlapStart(Vector2 overlapPos);
	protected:
		void _Draw_Impl() override;
	private:
		Vector2 m_overlapPos;
	};
}
