
#include "Runtime/Core/pch.h"
#include "MenuList.h"

#include <string>

#include "Runtime/Function/UI/ImGui/imgui.h"

LitchiRuntime::MenuList::MenuList(const std::string & p_name, bool p_locked) :
	name(p_name), locked(p_locked)
{
}

void LitchiRuntime::MenuList::_Draw_Impl()
{
	if (ImGui::BeginMenu(name.c_str(), !locked))
	{
		if (!m_opened)
		{
			ClickedEvent.Invoke();
			m_opened = true;
		}

		DrawWidgets();
		ImGui::EndMenu();
	}
	else
	{
		m_opened = false;
	}
}
