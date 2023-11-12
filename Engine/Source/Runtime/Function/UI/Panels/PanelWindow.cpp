
#include "PanelWindow.h"

#include "Runtime/Function/UI/ImGui/imgui_internal.h"
#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::PanelWindow::PanelWindow(const std::string& p_name, bool p_opened, const PanelWindowSettings& p_floatingPanelSettings) :
	name(p_name),
	resizable(p_floatingPanelSettings.resizable),
	closable(p_floatingPanelSettings.closable),
	movable(p_floatingPanelSettings.movable),
	scrollable(p_floatingPanelSettings.scrollable),
	dockable(p_floatingPanelSettings.dockable),
	hideBackground(p_floatingPanelSettings.hideBackground),
	forceHorizontalScrollbar(p_floatingPanelSettings.forceHorizontalScrollbar),
	forceVerticalScrollbar(p_floatingPanelSettings.forceVerticalScrollbar),
	allowHorizontalScrollbar(p_floatingPanelSettings.allowHorizontalScrollbar),
	bringToFrontOnFocus(p_floatingPanelSettings.bringToFrontOnFocus),
	collapsable(p_floatingPanelSettings.collapsable),
	allowInputs(p_floatingPanelSettings.allowInputs),
	m_opened(p_opened)
{
	autoSize = p_floatingPanelSettings.autoSize;
}

void LitchiRuntime::PanelWindow::Open()
{
	if (!m_opened)
	{
		m_opened = true;
		OpenEvent.Invoke();
	}
}

void LitchiRuntime::PanelWindow::Close()
{
	if (m_opened)
	{
		m_opened = false;
		CloseEvent.Invoke();
	}
}

void LitchiRuntime::PanelWindow::Focus()
{
	ImGui::SetWindowFocus((name + m_panelID).c_str());
}

void LitchiRuntime::PanelWindow::SetOpened(bool p_value)
{
	if (p_value != m_opened)
	{
		m_opened = p_value;

		if (m_opened)
			OpenEvent.Invoke();
		else
			CloseEvent.Invoke();
	}
}

bool LitchiRuntime::PanelWindow::IsOpened() const
{
	return m_opened;
}

bool LitchiRuntime::PanelWindow::IsHovered() const
{
	return m_hovered;
}

bool LitchiRuntime::PanelWindow::IsFocused() const
{
	return m_focused;
}

bool LitchiRuntime::PanelWindow::IsAppearing() const
{
	if (auto window = ImGui::FindWindowByName((name + GetPanelID()).c_str()); window)
		return window->Appearing;
	else
		return false;
}

void LitchiRuntime::PanelWindow::ScrollToBottom()
{
    m_mustScrollToBottom = true;
}

void LitchiRuntime::PanelWindow::ScrollToTop()
{
    m_mustScrollToTop = true;
}

bool LitchiRuntime::PanelWindow::IsScrolledToBottom() const
{
    return m_scrolledToBottom;
}

bool LitchiRuntime::PanelWindow::IsScrolledToTop() const
{
    return m_scrolledToTop;
}

void LitchiRuntime::PanelWindow::_Draw_Impl()
{
	if (m_opened)
	{
		int windowFlags = ImGuiWindowFlags_None;

		if (!resizable)					windowFlags |= ImGuiWindowFlags_NoResize;
		if (!movable)					windowFlags |= ImGuiWindowFlags_NoMove;
		if (!dockable)					windowFlags |= ImGuiWindowFlags_NoDocking;
		if (hideBackground)				windowFlags |= ImGuiWindowFlags_NoBackground;
		if (forceHorizontalScrollbar)	windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
		if (forceVerticalScrollbar)		windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
		if (allowHorizontalScrollbar)	windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
		if (!bringToFrontOnFocus)		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (!collapsable)				windowFlags |= ImGuiWindowFlags_NoCollapse;
		if (!allowInputs)				windowFlags |= ImGuiWindowFlags_NoInputs;
        if (!scrollable)                windowFlags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
		if (!titleBar)					windowFlags |= ImGuiWindowFlags_NoTitleBar;

		ImVec2 minSizeConstraint = Converter::ToImVec2(minSize);
		ImVec2 maxSizeConstraint = Converter::ToImVec2(maxSize);

		/* Cancel constraint if x or y is <= 0.f */
		if (minSizeConstraint.x <= 0.f || minSizeConstraint.y <= 0.f)
			minSizeConstraint = { 0.0f, 0.0f };

		if (maxSizeConstraint.x <= 0.f || maxSizeConstraint.y <= 0.f)
			maxSizeConstraint = { 10000.f, 10000.f };

		ImGui::SetNextWindowSizeConstraints(minSizeConstraint, maxSizeConstraint);

		if (ImGui::Begin((name + m_panelID).c_str(), closable ? &m_opened : nullptr, windowFlags))
		{
			m_hovered = ImGui::IsWindowHovered();
			m_focused = ImGui::IsWindowFocused();

            auto scrollY = ImGui::GetScrollY();

            m_scrolledToBottom = scrollY == ImGui::GetScrollMaxY();
            m_scrolledToTop = scrollY == 0.0f;

			if (!m_opened)
				CloseEvent.Invoke();

			OnDraw();

            if (m_mustScrollToBottom)
            {
                ImGui::SetScrollY(ImGui::GetScrollMaxY());
                m_mustScrollToBottom = false;
            }

            if (m_mustScrollToTop)
            {
                ImGui::SetScrollY(0.0f);
                m_mustScrollToTop = false;
            }

			DrawWidgets();
		}

		ImGui::End();
	}
}