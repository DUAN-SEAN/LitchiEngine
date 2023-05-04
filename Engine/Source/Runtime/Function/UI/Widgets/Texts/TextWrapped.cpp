
#include "TextWrapped.h"

LitchiRuntime::TextWrapped::TextWrapped(const std::string & p_content) :
	Text(p_content)
{
}

void LitchiRuntime::TextWrapped::_Draw_Impl()
{
	ImGui::TextWrapped(content.c_str());
}
