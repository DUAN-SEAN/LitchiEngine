
#include "RadioButton.h"

LitchiRuntime::RadioButton::RadioButton(bool p_selected, const std::string & p_label) :
	DataWidget<bool>(m_selected), label(p_label)
{
	if (p_selected)
		Select();
}

void LitchiRuntime::RadioButton::Select()
{
	m_selected = true;
	ClickedEvent.Invoke(m_radioID);
}

bool LitchiRuntime::RadioButton::IsSelected() const
{
	return m_selected;
}

bool LitchiRuntime::RadioButton::GetRadioID() const
{
	return m_radioID;
}

void LitchiRuntime::RadioButton::_Draw_Impl()
{
	if (ImGui::RadioButton((label + m_widgetID).c_str(), m_selected))
	{
		ClickedEvent.Invoke(m_radioID);
		this->NotifyChange();
	}
}
