/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "Image.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::Image::Image(uint32_t p_textureID, const Vector2& p_size) :
	textureID{ p_textureID }, size(p_size)
{
	
}

void LitchiRuntime::Image::_Draw_Impl()
{
	ImGui::Image(textureID.raw, Converter::ToImVec2(size), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
}
