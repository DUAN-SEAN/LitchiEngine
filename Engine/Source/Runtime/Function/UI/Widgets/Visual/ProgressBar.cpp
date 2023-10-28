/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "ProgressBar.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::ProgressBar::ProgressBar(float p_fraction, const Vector2 & p_size, const std::string & p_overlay) :
	fraction(p_fraction), size(p_size), overlay(p_overlay)
{
}

void LitchiRuntime::ProgressBar::_Draw_Impl()
{
	ImGui::ProgressBar(fraction, Converter::ToImVec2(size), !overlay.empty() ? overlay.c_str() : nullptr);
}
