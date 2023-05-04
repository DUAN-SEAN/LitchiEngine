/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Texts/TextLabelled.h"

OvUI::Widgets::Texts::TextLabelled::TextLabelled(const std::string& p_content, const std::string& p_label) :
	Text(p_content), label(p_label)
{
}

void OvUI::Widgets::Texts::TextLabelled::_Draw_Impl()
{
	ImGui::LabelText((label + m_widgetID).c_str(), content.c_str());
}
