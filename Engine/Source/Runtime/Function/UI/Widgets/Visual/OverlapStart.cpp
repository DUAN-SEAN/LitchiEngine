
#include "Runtime/Core/pch.h"
#include "OverlapStart.h"
#include "Separator.h"

LitchiRuntime::OverlapStart::OverlapStart(Vector2 overlapPos)
{
	m_overlapPos = overlapPos;
}

void LitchiRuntime::OverlapStart::_Draw_Impl()
{
	if (m_overlapPos != Vector2::Zero)
	{
		ImGui::SetCursorPos(ImVec2(m_overlapPos.x, m_overlapPos.y));
		ImGui::Separator();
	}

}
