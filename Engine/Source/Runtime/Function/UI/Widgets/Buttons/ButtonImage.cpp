
#include "ButtonImage.h"

#include "Runtime/Function/UI/ImGui/Source/imgui_internal.h"
#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::ButtonImage::ButtonImage(uint32_t p_textureID, const glm::vec2 & p_size) :
	textureID{ p_textureID }, size(p_size)
{
}

void LitchiRuntime::ButtonImage::_Draw_Impl()
{
	ImVec4 bg = Converter::ToImVec4(background);
	ImVec4 tn = Converter::ToImVec4(tint);

	if (ImGui::ImageButton(textureID.raw, Converter::ToImVec2(size), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f), -1, bg, tn, disabled ? ImGuiButtonFlags_Disabled : 0))
		ClickedEvent.Invoke();
}
