
#include "Button.h"


#include "Runtime/Function/UI/ImGui/imgui.h"
#include "Runtime/Function/UI/ImGui/imgui_internal.h"
#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::Button::Button(const std::string& p_label, const Vector2& p_size, bool p_disabled) :
	label(p_label), size(p_size), disabled(p_disabled)
{
	auto& style = ImGui::GetStyle();

	idleBackgroundColor		= Converter::ToColor(style.Colors[ImGuiCol_Button]);
	hoveredBackgroundColor	= Converter::ToColor(style.Colors[ImGuiCol_ButtonHovered]);
	clickedBackgroundColor	= Converter::ToColor(style.Colors[ImGuiCol_ButtonActive]);
	textColor				= Converter::ToColor(style.Colors[ImGuiCol_Text]);
}

void LitchiRuntime::Button::_Draw_Impl()
{
	auto& style = ImGui::GetStyle();

	auto defaultIdleColor		= style.Colors[ImGuiCol_Button];
	auto defaultHoveredColor	= style.Colors[ImGuiCol_ButtonHovered];
	auto defaultClickedColor	= style.Colors[ImGuiCol_ButtonActive];
	auto defaultTextColor		= style.Colors[ImGuiCol_Text];

	style.Colors[ImGuiCol_Button]			= Converter::ToImVec4(idleBackgroundColor);
	style.Colors[ImGuiCol_ButtonHovered]	= Converter::ToImVec4(hoveredBackgroundColor);
	style.Colors[ImGuiCol_ButtonActive]		= Converter::ToImVec4(clickedBackgroundColor);
	style.Colors[ImGuiCol_Text]				= Converter::ToImVec4(textColor);

	if (ImGui::ButtonEx((label + m_widgetID).c_str(), Converter::ToImVec2(size), disabled ? ImGuiButtonFlags_::ImGuiButtonFlags_None : 0))
		ClickedEvent.Invoke();

	style.Colors[ImGuiCol_Button]			= defaultIdleColor;
	style.Colors[ImGuiCol_ButtonHovered]	= defaultHoveredColor;
	style.Colors[ImGuiCol_ButtonActive]		= defaultClickedColor;
	style.Colors[ImGuiCol_Text]				= defaultTextColor;
}
