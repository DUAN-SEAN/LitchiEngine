
#include "Runtime/Core/pch.h"
#include "TreeNode.h"

LitchiRuntime::TreeNode::TreeNode(const std::string & p_name, bool p_arrowClickToOpen,bool defaultOpen) :
	DataWidget(name),
	name(p_name),
	m_arrowClickToOpen(p_arrowClickToOpen),
	m_defaultOpen(defaultOpen)
{
	m_autoExecutePlugins = false;
}

void LitchiRuntime::TreeNode::Open()
{
	m_shouldOpen = true;
	m_shouldClose = false;
}

void LitchiRuntime::TreeNode::Close()
{
	m_shouldClose = true;
	m_shouldOpen = false;
}

bool LitchiRuntime::TreeNode::IsOpened() const
{
	return m_opened;
}

void LitchiRuntime::TreeNode::_Draw_Impl()
{
	bool prevOpened = m_opened;

	if (m_shouldOpen)
	{
		//ImGui::SetNextTreeNodeOpen(true);
		ImGui::SetNextItemOpen(true,0);
		m_shouldOpen = false;
	}
	else if (m_shouldClose)
	{
		// ImGui::SetNextTreeNodeOpen(false);
		ImGui::SetNextItemOpen(false, 0);
		m_shouldClose = false;
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	if (m_arrowClickToOpen) flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	if (selected)			flags |= ImGuiTreeNodeFlags_Selected;
	if (leaf)				flags |= ImGuiTreeNodeFlags_Leaf;
	if (m_defaultOpen) flags |= ImGuiTreeNodeFlags_DefaultOpen;

	bool opened = ImGui::TreeNodeEx((name + m_widgetID).c_str(), flags);

    if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
    {
        ClickedEvent.Invoke();

        if (ImGui::IsMouseDoubleClicked(0))
        {
            DoubleClickedEvent.Invoke();
        }
    }

	if (opened)
	{
		if (!prevOpened)
			OpenedEvent.Invoke();

		m_opened = true;

		ExecutePlugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs

		DrawWidgets();

		ImGui::TreePop();
	}
	else
	{
		if (prevOpened)
			ClosedEvent.Invoke();

		m_opened = false;

		ExecutePlugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs
	}
}
