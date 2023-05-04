
#include "Text.h"

LitchiRuntime::Text::Text(const std::string & p_content) :
	DataWidget(content),
	content(p_content)
{
}

void LitchiRuntime::Text::_Draw_Impl()
{
	ImGui::Text(content.c_str());
}
