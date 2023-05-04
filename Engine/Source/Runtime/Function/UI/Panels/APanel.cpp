
#include <algorithm>

#include "APanel.h"

uint64_t LitchiRuntime::APanel::__PANEL_ID_INCREMENT = 0;

LitchiRuntime::APanel::APanel()
{
	m_panelID = "##" + std::to_string(__PANEL_ID_INCREMENT++);
}

void LitchiRuntime::APanel::Draw()
{
	if (enabled)
		_Draw_Impl();
}

const std::string & LitchiRuntime::APanel::GetPanelID() const
{
	return m_panelID;
}
