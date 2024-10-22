
#include "Runtime/Core/pch.h"
#include "ButtonSmall.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::ButtonSmall::ButtonSmall(const std::string& p_label) :
	label(p_label)
{
	auto& style = ImGui::GetStyle();

	idleBackgroundColor = Converter::ToColor(style.Colors[ImGuiCol_Button]);
	hoveredBackgroundColor = Converter::ToColor(style.Colors[ImGuiCol_ButtonHovered]);
	clickedBackgroundColor = Converter::ToColor(style.Colors[ImGuiCol_ButtonActive]);
	textColor = Converter::ToColor(style.Colors[ImGuiCol_Text]);
}

void LitchiRuntime::ButtonSmall::_Draw_Impl()
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

	if (ImGui::SmallButton((label + m_widgetID).c_str()))
		ClickedEvent.Invoke();

	style.Colors[ImGuiCol_Button]			= defaultIdleColor;
	style.Colors[ImGuiCol_ButtonHovered]	= defaultHoveredColor;
	style.Colors[ImGuiCol_ButtonActive]		= defaultClickedColor;
	style.Colors[ImGuiCol_Text]				= defaultTextColor;
}
