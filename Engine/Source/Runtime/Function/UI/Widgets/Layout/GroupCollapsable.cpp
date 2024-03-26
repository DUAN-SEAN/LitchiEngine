
#include "Runtime/Core/pch.h"
#include "GroupCollapsable.h"

LitchiRuntime::GroupCollapsable::GroupCollapsable(const std::string & p_name) :
	name(p_name)
{
}

void LitchiRuntime::GroupCollapsable::_Draw_Impl()
{
	bool previouslyOpened = opened;

	if (ImGui::CollapsingHeader(name.c_str(), closable ? &opened : nullptr))
		Group::_Draw_Impl();

	if (opened != previouslyOpened)
	{
		if (opened)
			OpenEvent.Invoke();
		else
			CloseEvent.Invoke();
	}
}
