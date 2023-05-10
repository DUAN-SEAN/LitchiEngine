
#include "Editor/include/Panels/Hierarchy.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/UI/Plugins/ContextualMenu.h"
#include "Runtime/Function/UI/Plugins/DDSource.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputText.h"
#include "Runtime/Function/UI/Widgets/Layout/TreeNode.h"

using namespace LitchiRuntime;

class HierarchyContextualMenu : public LitchiRuntime::ContextualMenu
{
public:
	HierarchyContextualMenu(LitchiRuntime::GameObject* p_target, TreeNode& p_treeNode, bool p_panelMenu = false) :
		m_target(p_target),
		m_treeNode(p_treeNode)
	{
		/* if (m_target)
		 {
			 auto& focusButton = CreateWidget<MenuItem>("Focus");
			 focusButton.ClickedEvent += [this]
			 {
				 EDITOR_EXEC(MoveToTarget(*m_target));
			 };

			 auto& duplicateButton = CreateWidget<MenuItem>("Duplicate");
			 duplicateButton.ClickedEvent += [this]
			 {
				 EDITOR_EXEC(DelayAction(EDITOR_BIND(DuplicateActor, std::ref(*m_target), nullptr, true), 0));
			 };

			 auto& deleteButton = CreateWidget<MenuItem>("Delete");
			 deleteButton.ClickedEvent += [this]
			 {
				 EDITOR_EXEC(DestroyActor(std::ref(*m_target)));
			 };
		 }

		 auto& createActor = CreateWidget<MenuList>("Create...");
		 OvEditor::Utils::ActorCreationMenu::GenerateActorCreationMenu(createActor, m_target, std::bind(&TreeNode::Open, &m_treeNode));*/
	}

	virtual void Execute() override
	{
		if (m_widgets.size() > 0)
			ContextualMenu::Execute();
	}

private:
	GameObject* m_target;
	TreeNode& m_treeNode;
};

void ExpandTreeNode(TreeNode& p_toExpand, const TreeNode* p_root)
{
	p_toExpand.Open();

	if (&p_toExpand != p_root && p_toExpand.HasParent())
	{
		ExpandTreeNode(*static_cast<TreeNode*>(p_toExpand.GetParent()), p_root);
	}
}

std::vector<TreeNode*> nodesToCollapse;
std::vector<TreeNode*> founds;

void ExpandTreeNodeAndEnable(TreeNode& p_toExpand, const TreeNode* p_root)
{
	if (!p_toExpand.IsOpened())
	{
		p_toExpand.Open();
		nodesToCollapse.push_back(&p_toExpand);
	}

	p_toExpand.enabled = true;

	if (&p_toExpand != p_root && p_toExpand.HasParent())
	{
		ExpandTreeNodeAndEnable(*static_cast<TreeNode*>(p_toExpand.GetParent()), p_root);
	}
}

LitchiEditor::Hierarchy::Hierarchy
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	auto& searchBar = CreateWidget<InputText>();
	searchBar.ContentChangedEvent += [this](const std::string& p_content)
	{
		founds.clear();
		auto content = p_content;
		std::transform(content.begin(), content.end(), content.begin(), ::tolower);

		for (auto& [actor, item] : m_widgetActorLink)
		{
			if (!p_content.empty())
			{
				auto itemName = item->name;
				std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

				if (itemName.find(content) != std::string::npos)
				{
					founds.push_back(item);
				}

				item->enabled = false;
			}
			else
			{
				item->enabled = true;
			}
		}

		for (auto node : founds)
		{
			node->enabled = true;

			if (node->HasParent())
			{
				ExpandTreeNodeAndEnable(*static_cast<TreeNode*>(node->GetParent()), m_sceneRoot);
			}
		}

		if (p_content.empty())
		{
			for (auto node : nodesToCollapse)
			{
				node->Close();
			}

			nodesToCollapse.clear();
		}
	};

	m_sceneRoot = &CreateWidget<TreeNode>("Root", true);
	static_cast<TreeNode*>(m_sceneRoot)->Open();
	m_sceneRoot->AddPlugin<DDTarget<std::pair<GameObject*, TreeNode*>>>("Actor").DataReceivedEvent += [this](std::pair<GameObject*, TreeNode*> p_element)
	{
		if (p_element.second->HasParent())
			p_element.second->GetParent()->UnconsiderWidget(*p_element.second);

		m_sceneRoot->ConsiderWidget(*p_element.second);

		// p_element.first->DetachFromParent();
	};

	m_sceneRoot->AddPlugin<HierarchyContextualMenu>(nullptr, *m_sceneRoot);

	/*EDITOR_EVENT(ActorUnselectedEvent) += std::bind(&Hierarchy::UnselectActorsWidgets, this);
	EDITOR_CONTEXT(sceneManager).SceneUnloadEvent += std::bind(&Hierarchy::Clear, this);
	OvCore::ECS::Actor::CreatedEvent += std::bind(&Hierarchy::AddActorByInstance, this, std::placeholders::_1);
	OvCore::ECS::Actor::DestroyedEvent += std::bind(&Hierarchy::DeleteActorByInstance, this, std::placeholders::_1);
	EDITOR_EVENT(ActorSelectedEvent) += std::bind(&Hierarchy::SelectActorByInstance, this, std::placeholders::_1);
	OvCore::ECS::Actor::AttachEvent += std::bind(&Hierarchy::AttachActorToParent, this, std::placeholders::_1);
	OvCore::ECS::Actor::DettachEvent += std::bind(&Hierarchy::DetachFromParent, this, std::placeholders::_1);*/
}

void LitchiEditor::Hierarchy::Clear()
{
	// EDITOR_EXEC(UnselectActor());

	m_sceneRoot->RemoveAllWidgets();
	m_widgetActorLink.clear();
}

void LitchiEditor::Hierarchy::UnselectActorsWidgets()
{
	for (auto& widget : m_widgetActorLink)
		widget.second->selected = false;
}

void LitchiEditor::Hierarchy::SelectActorByInstance(GameObject* p_actor)
{
	if (auto result = m_widgetActorLink.find(p_actor); result != m_widgetActorLink.end())
		if (result->second)
			SelectActorByWidget(*result->second);
}

void LitchiEditor::Hierarchy::SelectActorByWidget(TreeNode& p_widget)
{
	UnselectActorsWidgets();

	p_widget.selected = true;

	if (p_widget.HasParent())
	{
		ExpandTreeNode(*static_cast<TreeNode*>(p_widget.GetParent()), m_sceneRoot);
	}
}

void LitchiEditor::Hierarchy::AttachActorToParent(GameObject* p_actor)
{
	auto actorWidget = m_widgetActorLink.find(p_actor);

	if (actorWidget != m_widgetActorLink.end())
	{
		auto widget = actorWidget->second;

		if (widget->HasParent())
			widget->GetParent()->UnconsiderWidget(*widget);

		if (p_actor->HasParent())
		{
			auto parentWidget = m_widgetActorLink.at(p_actor->GetParent());
			parentWidget->leaf = false;
			parentWidget->ConsiderWidget(*widget);
		}
	}
}

void LitchiEditor::Hierarchy::DetachFromParent(GameObject* p_actor)
{
	if (auto actorWidget = m_widgetActorLink.find(p_actor); actorWidget != m_widgetActorLink.end())
	{
		if (p_actor->HasParent() && p_actor->GetParent()->GetChildren().size() == 1)
		{
			if (auto parentWidget = m_widgetActorLink.find(p_actor->GetParent()); parentWidget != m_widgetActorLink.end())
			{
				parentWidget->second->leaf = true;
			}
		}

		auto widget = actorWidget->second;

		if (widget->HasParent())
			widget->GetParent()->UnconsiderWidget(*widget);

		m_sceneRoot->ConsiderWidget(*widget);
	}
}

void LitchiEditor::Hierarchy::DeleteActorByInstance(GameObject* p_actor)
{
	if (auto result = m_widgetActorLink.find(p_actor); result != m_widgetActorLink.end())
	{
		if (result->second)
		{
			result->second->Destroy();
		}

		m_widgetActorLink.erase(result);
	}
}

void LitchiEditor::Hierarchy::AddActorByInstance(GameObject* p_actor)
{
	auto& textSelectable = m_sceneRoot->CreateWidget<TreeNode>(p_actor->name(), true);
	textSelectable.leaf = true;
	textSelectable.AddPlugin<HierarchyContextualMenu>(p_actor, textSelectable);
	textSelectable.AddPlugin<DDSource<std::pair<GameObject*, TreeNode*>>>("Actor", "Attach to...", std::make_pair(p_actor, &textSelectable));
	textSelectable.AddPlugin<DDTarget<std::pair<GameObject*, TreeNode*>>>("Actor").DataReceivedEvent += [p_actor, &textSelectable](std::pair<GameObject*, TreeNode*> p_element)
	{
		if (p_element.second->HasParent())
			p_element.second->GetParent()->UnconsiderWidget(*p_element.second);

		textSelectable.ConsiderWidget(*p_element.second);

		p_element.first->SetParent(p_actor);
	};
	auto& dispatcher = textSelectable.AddPlugin<DataDispatcher<std::string>>();

	GameObject* targetPtr = p_actor;
	dispatcher.RegisterGatherer([targetPtr] { return targetPtr->name(); });

	m_widgetActorLink[targetPtr] = &textSelectable;

	// 暂时不用点击和双击事件 回头再加
	textSelectable.ClickedEvent += std::bind(&ApplicationEditor::SelectActor, ApplicationEditor::Instance(), p_actor);
	// textSelectable.DoubleClickedEvent += EDITOR_BIND(MoveToTarget, std::ref(p_actor));
}