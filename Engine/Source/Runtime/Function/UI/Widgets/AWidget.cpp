
#include "Runtime/Core/pch.h"
#include "AWidget.h"

uint64_t LitchiRuntime::AWidget::__WIDGET_ID_INCREMENT = 0;

LitchiRuntime::AWidget::AWidget()
{
	m_widgetID = "##" + std::to_string(__WIDGET_ID_INCREMENT++);
}

void LitchiRuntime::AWidget::LinkTo(const AWidget& p_widget)
{
	m_widgetID = p_widget.m_widgetID;
}

void LitchiRuntime::AWidget::Destroy()
{
	m_destroyed = true;
}

bool LitchiRuntime::AWidget::IsDestroyed() const
{
	return m_destroyed;
}

void LitchiRuntime::AWidget::SetParent(WidgetContainer * p_parent)
{
	m_parent = p_parent;
}

bool LitchiRuntime::AWidget::HasParent() const
{
	return m_parent;
}

LitchiRuntime::WidgetContainer * LitchiRuntime::AWidget::GetParent()
{
	return m_parent;
}

void LitchiRuntime::AWidget::Draw()
{
	if (enabled)
	{
		_Draw_Impl();

		if (m_autoExecutePlugins)
			ExecutePlugins();

		if (!lineBreak)
			ImGui::SameLine();
	}
}
