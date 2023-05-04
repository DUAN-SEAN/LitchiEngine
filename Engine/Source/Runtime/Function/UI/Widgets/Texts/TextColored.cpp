
#include "TextColored.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::TextColored::TextColored(const std::string& p_content, const Color& p_color) :
	Text(p_content), color(p_color)
{
}

void LitchiRuntime::TextColored::_Draw_Impl()
{
	ImGui::TextColored(Converter::ToImVec4(color), content.c_str());
}
