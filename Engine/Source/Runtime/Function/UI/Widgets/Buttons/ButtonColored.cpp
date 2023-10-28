
#include "ButtonColored.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::ButtonColored::ButtonColored(const std::string & p_label, const Color& p_color, const Vector2& p_size, bool p_enableAlpha) :
	label(p_label), color(p_color), size(p_size), enableAlpha(p_enableAlpha)
{
}

void LitchiRuntime::ButtonColored::_Draw_Impl()
{
	ImVec4 imColor = Converter::ToImVec4(color);

	if (ImGui::ColorButton((label + m_widgetID).c_str(), imColor, !enableAlpha ? ImGuiColorEditFlags_NoAlpha : 0, Converter::ToImVec2(size)))
		ClickedEvent.Invoke();

	color = Converter::ToColor(imColor);
}
