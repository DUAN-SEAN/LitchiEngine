/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Texts/Text.h"

OvUI::Widgets::Texts::Text::Text(const std::string & p_content) :
	DataWidget(content),
	content(p_content)
{
}

void OvUI::Widgets::Texts::Text::_Draw_Impl()
{
	ImGui::Text(content.c_str());
}
