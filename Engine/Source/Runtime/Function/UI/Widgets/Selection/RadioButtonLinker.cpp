
#include "Runtime/Core/pch.h"
#include "RadioButtonLinker.h"

#include <algorithm>

LitchiRuntime::RadioButtonLinker::RadioButtonLinker() : DataWidget<int>(m_selected)
{
}

void LitchiRuntime::RadioButtonLinker::Link(RadioButton& p_radioButton)
{
	p_radioButton.m_radioID = m_availableRadioID++;
	auto listenerID = p_radioButton.ClickedEvent += std::bind(&RadioButtonLinker::OnRadioButtonClicked, this, std::placeholders::_1);

	if (p_radioButton.IsSelected() && m_selected == -1)
		m_selected = p_radioButton.m_radioID;

	m_radioButtons.emplace_back(listenerID, std::ref(p_radioButton));
}

void LitchiRuntime::RadioButtonLinker::Unlink(RadioButton& p_radioButton)
{
	auto it = std::find_if(m_radioButtons.begin(), m_radioButtons.end(), [&p_radioButton](std::pair<ListenerID, std::reference_wrapper<RadioButton>>& p_pair)
	{
		return &p_pair.second.get() == &p_radioButton;
	});

	if (it != m_radioButtons.end())
	{
		it->second.get().ClickedEvent.RemoveListener(it->first);
		m_radioButtons.erase(it);
	}
}

int LitchiRuntime::RadioButtonLinker::GetSelected() const
{
	return m_selected;
}

void LitchiRuntime::RadioButtonLinker::_Draw_Impl()
{
	// The RadioButtonLinker is special, it has nothing to display :)
}

void LitchiRuntime::RadioButtonLinker::OnRadioButtonClicked(int p_radioID)
{
	if (p_radioID != m_selected)
	{
		m_selected = p_radioID;
		ValueChangedEvent.Invoke(m_selected);
		this->NotifyChange();

		for (const auto&[listener, radioButton] : m_radioButtons)
			radioButton.get().m_selected = radioButton.get().m_radioID == m_selected;
	}
}
