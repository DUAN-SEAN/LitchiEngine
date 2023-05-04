
#include "ButtonArrow.h"

LitchiRuntime::ButtonArrow::ButtonArrow(ImGuiDir p_direction) :
	direction(p_direction)
{
}

void LitchiRuntime::ButtonArrow::_Draw_Impl()
{
	if (ImGui::ArrowButton(m_widgetID.c_str(), direction))
		ClickedEvent.Invoke();
}
