
#include "ContextualMenu.h"

#include "Runtime/Function/UI/ImGui/imgui.h"

void LitchiRuntime::ContextualMenu::Execute()
{
	if (ImGui::BeginPopupContextItem())
	{
		DrawWidgets();
		ImGui::EndPopup();
	}
}

void LitchiRuntime::ContextualMenu::Close()
{
	ImGui::CloseCurrentPopup();
}
