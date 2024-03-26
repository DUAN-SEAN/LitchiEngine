
#include "Runtime/Core/pch.h"
#include "TextDisabled.h"

LitchiRuntime::TextDisabled::TextDisabled(const std::string & p_content) :
	Text(p_content)
{
}

void LitchiRuntime::TextDisabled::_Draw_Impl()
{
	ImGui::TextDisabled(content.c_str());
}
