
#include "Runtime/Core/pch.h"
#include "Editor/include/Panels/Hierarchy.h"

#include "Editor/include/ApplicationEditor.h"
#include "Editor/include/Menus/ActorCreationMenu.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/UI/Plugins/ContextualMenu.h"
#include "Runtime/Function/UI/Plugins/DDSource.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputText.h"
#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Function/UI/Widgets/Layout/TreeNode.h"

using namespace LitchiEditor;
using namespace LitchiRuntime;

class HierarchyContextualMenu : public LitchiRuntime::ContextualMenu
{
public:
	HierarchyContextualMenu(LitchiRuntime::GameObject* p_target, TreeNode& p_treeNode, bool p_panelMenu = false) :
		m_target(p_target),
		m_treeNode(p_treeNode)
	{
		 // todo:
		 if (m_target)
		 {
			 auto& focusButton = CreateWidget<MenuItem>("Focus");
			 focusButton.ClickedEvent += [this]
			 {
				  ApplicationEditor::Instance()->MoveToTarget(m_target);
			 };

			 auto& duplicateButton = CreateWidget<MenuItem>("Duplicate");
			 duplicateButton.ClickedEvent += [this]
			 {
				 EDITOR_EXEC(DelayAction(EDITOR_BIND(DuplicateActor, m_target, nullptr, true), 0));
			 };

			 auto& deleteButton = CreateWidget<MenuItem>("Delete");
			 deleteButton.ClickedEvent += [this]
			 {
				 EDITOR_EXEC(DestroyActor(m_target));
			 };
		 }

		 auto& createActor = CreateWidget<MenuList>("Create...");
		 ActorCreationMenu::GenerateActorCreationMenu(createActor, m_target, std::bind(&TreeNode::Open, &m_treeNode));
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

		p_element.first->SetParent(nullptr);
	};

	m_sceneRoot->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this](auto p_receivedData)
	{
		// 先构建所有的叶子
		auto* scene = ApplicationEditor::Instance()->sceneManager->GetCurrentScene();
		LoadPrefabFromFile(scene, nullptr, p_receivedData.first);
	};
	
	m_sceneRoot->AddPlugin<HierarchyContextualMenu>(nullptr, *m_sceneRoot);

	/*EDITOR_EVENT(ActorUnselectedEvent) += std::bind(&Hierarchy::UnselectActorsWidgets, this);
	EDITOR_CONTEXT(sceneManager).SceneUnloadEvent += std::bind(&Hierarchy::Clear, this);
	GameObject::CreatedEvent += std::bind(&Hierarchy::AddActorByInstance, this, std::placeholders::_1);
	GameObject::DestroyedEvent += std::bind(&Hierarchy::DeleteActorByInstance, this, std::placeholders::_1);
	EDITOR_EVENT(ActorSelectedEvent) += std::bind(&Hierarchy::SelectActorByInstance, this, std::placeholders::_1);
	GameObject::AttachEvent += std::bind(&Hierarchy::AttachActorToParent, this, std::placeholders::_1);
	GameObject::DettachEvent += std::bind(&Hierarchy::DetachFromParent, this, std::placeholders::_1);*/


	GameObject::CreatedEvent += std::bind(&Hierarchy::AddActorByInstance, this, std::placeholders::_1);
	GameObject::DestroyedEvent += std::bind(&Hierarchy::DeleteActorByInstance, this, std::placeholders::_1);
	GameObject::AttachEvent += std::bind(&Hierarchy::AttachActorToParent, this, std::placeholders::_1);
	GameObject::DettachEvent += std::bind(&Hierarchy::DetachFromParent, this, std::placeholders::_1);
	Scene::InstantiatePrefabEvent += std::bind(&Hierarchy::AddActorByPrefabInstance, this, std::placeholders::_1);
	ServiceLocator::Get<SceneManager>().SceneLoadEvent += std::bind(&Hierarchy::OnSceneLoad, this);
	//ServiceLocator::Get<SceneManager>().SceneUnloadEvent += std::bind(&Hierarchy::OnSceneUnLoad, this);
}

void LitchiEditor::Hierarchy::Refresh()
{
	// 清理
	Clear();

	 // 先构建所有的叶子
	auto* scene = ApplicationEditor::Instance()->sceneManager->GetCurrentScene();
	if(scene == nullptr)
	{
		return;
	}

	m_sceneRoot->name = scene->GetName();

	for (auto go : scene->m_gameObjectList)
	{
		if(go != nullptr)
		{
			AddActorByInstance(go);
		}
	}

	// 根据父子关系重构树状结构
	for (auto go : scene->m_gameObjectList)
	{
		if (go != nullptr && go->HasParent())
		{
			AttachActorToParent(go);
		}
	}

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
		}else
		{
			m_sceneRoot->ConsiderWidget(*widget);
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
	auto* scene = ServiceLocator::Get<SceneManager>().GetCurrentScene();
	auto inSceneActor= scene->Find(p_actor->m_id);
	if(inSceneActor ==nullptr || inSceneActor != p_actor)
	{
		return;
	}

	auto& textSelectable = m_sceneRoot->CreateWidget<TreeNode>(p_actor->GetName(), true);
	textSelectable.leaf = true;
	textSelectable.AddPlugin<HierarchyContextualMenu>(p_actor, textSelectable);
	// textSelectable.AddPlugin<DDSource<std::pair<Scene*, GameObject*>>>("CreatePrefab", "Create Prefab...", std::make_pair(p_actor->GetScene(), p_actor));// cant run source only one
	textSelectable.AddPlugin<DDSource<std::pair<GameObject*, TreeNode*>>>("Actor", p_actor->GetName(), std::make_pair(p_actor, &textSelectable));
	textSelectable.AddPlugin<DDTarget<std::pair<GameObject*, TreeNode*>>>("Actor").DataReceivedEvent += [p_actor, &textSelectable](std::pair<GameObject*, TreeNode*> p_element)
	{
		if (p_element.second->HasParent())
			p_element.second->GetParent()->UnconsiderWidget(*p_element.second);

		textSelectable.ConsiderWidget(*p_element.second);

		p_element.first->SetParent(p_actor);
	};
	auto& dispatcher = textSelectable.AddPlugin<DataDispatcher<std::string>>();

	GameObject* targetPtr = p_actor;
	dispatcher.RegisterGatherer([targetPtr] { return targetPtr->GetName(); });

	m_widgetActorLink[targetPtr] = &textSelectable;

	// 暂时不用点击和双击事件 回头再加
	textSelectable.ClickedEvent += std::bind(&ApplicationEditor::SelectActor, ApplicationEditor::Instance(), p_actor);
	textSelectable.DoubleClickedEvent += [this, p_actor]
	{
		ApplicationEditor::Instance()->MoveToTarget(p_actor);
	};// 将相机对焦到物体


	textSelectable.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this,p_actor](auto p_receivedData)
		{
			// 先构建所有的叶子
			auto* scene = ApplicationEditor::Instance()->sceneManager->GetCurrentScene();
			LoadPrefabFromFile(scene,p_actor, p_receivedData.first);
		
		};

}

void LitchiEditor::Hierarchy::AddActorByPrefabInstance(GameObject* p_actor)
{
	// just refresh all root
	Refresh();
}

void LitchiEditor::Hierarchy::OnSceneLoad()
{
	// just refresh all root
	Refresh();
}

void LitchiEditor::Hierarchy::OnSceneUnLoad()
{
	// just refresh all root
	Refresh();
}

void LitchiEditor::Hierarchy::LoadPrefabFromFile(Scene* scene, GameObject* root, std::string filePath)
{

	std::string itemname = PathParser::GetElementName(filePath);
	PathParser::EFileType fileType = PathParser::GetFileType(itemname);

	Prefab* prefab = nullptr;
	if (fileType == PathParser::EFileType::PREFAB)
	{
		prefab = ApplicationBase::Instance()->prefabManager->LoadResource(filePath);
		prefab->PostResourceLoaded();
	}

	if (fileType == PathParser::EFileType::MODEL)
	{
		auto model = ApplicationBase::Instance()->modelManager->LoadResource(filePath);
		prefab = model->GetModelPrefab();
	}

	if (prefab)
	{
		auto prefabObj = EDITOR_EXEC(LoadPrefab(scene, root, prefab));

		//AddActorByInstance(prefabObj);
		//AttachActorToParent(prefabObj);
		EDITOR_EXEC(SelectActor(prefabObj));
	}

	//Refresh();
}
