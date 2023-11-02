
#include "Image.h"

#include "Runtime/Function/UI/ImGui/Extensions/ImGuiExtension.h"
#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::Image::Image(uint32_t p_textureID, const Vector2& p_size) :
	textureID{ p_textureID }, size(p_size), renderTarget{nullptr}
{
	
}

LitchiRuntime::Image::Image(RHI_Texture* p_renderTarget, const Vector2& p_size) :
	renderTarget{ p_renderTarget }, size(p_size), textureID{}
{
}

void LitchiRuntime::Image::_Draw_Impl()
{
	if(renderTarget)
	{
		ImGuiSp::image(renderTarget, ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)));
	}
	else
	{
		ImGui::Image(textureID.raw, Converter::ToImVec2(size), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
	}
}
