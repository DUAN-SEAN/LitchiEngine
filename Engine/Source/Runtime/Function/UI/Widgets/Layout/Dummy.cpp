
#include "Runtime/Core/pch.h"
#include "Dummy.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::Dummy::Dummy(const Vector2& p_size) : size(p_size)
{
}

void LitchiRuntime::Dummy::_Draw_Impl()
{
	ImGui::Dummy(Converter::ToImVec2(size));
}
