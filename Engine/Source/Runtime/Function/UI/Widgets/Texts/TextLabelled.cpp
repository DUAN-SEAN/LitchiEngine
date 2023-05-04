
#include "TextLabelled.h"

LitchiRuntime::TextLabelled::TextLabelled(const std::string& p_content, const std::string& p_label) :
	Text(p_content), label(p_label)
{
}

void LitchiRuntime::TextLabelled::_Draw_Impl()
{
	ImGui::LabelText((label + m_widgetID).c_str(), content.c_str());
}
