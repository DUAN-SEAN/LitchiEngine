
#pragma once

#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Widgets/Layout/TreeNode.h"

namespace LitchiRuntime
{
	class GameObject;
}

using namespace LitchiRuntime;

namespace LitchiEditor
{
	class Hierarchy : public LitchiRuntime::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Hierarchy
		(
			const std::string& p_title,
			bool p_opened,
			const PanelWindowSettings& p_windowSettings
		);

		/**
		 * Refresh hierarchy 
		 */
		void Refresh();

		/**
		* Clear hierarchy nodes
		*/
		void Clear();

		/**
		* Unselect every widgets
		*/
		void UnselectActorsWidgets();

		/**
		* Select the widget corresponding to the given actor
		* @param p_actor
		*/
		void SelectActorByInstance(GameObject* p_actor);

		/**
		* Select the widget
		* @param p_actor
		*/
		void SelectActorByWidget(TreeNode& p_widget);

		/**
		* Attach the given actor linked widget to its parent widget
		* @param p_actor
		*/
		void AttachActorToParent(GameObject* p_actor);

		/**
		* Detach the given actor linked widget from its parent widget
		* @param p_actor
		*/
		void DetachFromParent(GameObject* p_actor);

		/**
		* Delete the widget referencing the given actor
		* @param p_actor
		*/
		void DeleteActorByInstance(GameObject* p_actor);

		/**
		* Add a widget referencing the given actor
		* @param p_actor
		*/
		void AddActorByInstance(GameObject* p_actor);

	public:
		Event<GameObject*> ActorSelectedEvent;
		Event<GameObject*> ActorUnselectedEvent;

	private:
		void LoadPrefabFromFile(Scene* scene, GameObject* root, std::string filePath);

		TreeNode* m_sceneRoot;

		std::unordered_map<GameObject*, TreeNode*> m_widgetActorLink;
	};
}
