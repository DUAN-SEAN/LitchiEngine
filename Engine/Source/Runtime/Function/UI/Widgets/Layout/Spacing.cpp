
#include "Runtime/Core/pch.h"
#include "Spacing.h"

LitchiRuntime::Spacing::Spacing(uint16_t p_spaces) : spaces(p_spaces)
{
}

void LitchiRuntime::Spacing::_Draw_Impl()
{
	for (uint16_t i = 0; i < spaces; ++i)
	{
		ImGui::Spacing();

		if (i + 1 < spaces)
			ImGui::SameLine();
	}
}
