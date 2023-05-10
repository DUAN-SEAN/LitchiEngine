
#pragma once
#include <string>

#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"

namespace LitchiEditor
{
	class Inspector : publicPanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Inspector
		(
			const std::string& p_title,
			bool p_opened,
			const LitchiRuntime::PanelWindowSettings& p_windowSettings
		);

		/**
		* Destructor
		*/
		~Inspector();

		/**
		* Focus the given actor
		* @param p_target
		*/
		void FocusActor(OvCore::ECS::Actor& p_target);

		/**
		* Unfocus the currently targeted actor
		*/
		void UnFocus();

		/**
		* Unfocus the currently targeted actor without removing listeners attached to this actor
		*/
		void SoftUnFocus();

		/**
		* Returns the currently selected actor
		*/
		GameObject* GetTargetActor() const;

		/**
		* Create the actor inspector for the given actor
		*/
		void CreateActorInspector(OvCore::ECS::Actor& p_target);

		/**
		* Draw the given component in inspector
		*/
		void DrawComponent(OvCore::ECS::Components::AComponent& p_component);

		/**
		* Draw the given behaviour in inspector
		*/
		void DrawBehaviour(OvCore::ECS::Components::Behaviour& p_behaviour);

		/**
		* Refresh the inspector
		*/
		void Refresh();

	private:
		GameObject* m_targetActor = nullptr;
		Group* m_actorInfo;
		Group* m_inspectorHeader;
		OvUI::Widgets::Selection::ComboBox* m_componentSelectorWidget;
        InputText* m_scriptSelectorWidget;

		uint64_t m_componentAddedListener	= 0;
		uint64_t m_componentRemovedListener = 0;
		uint64_t m_behaviourAddedListener	= 0;
		uint64_t m_behaviourRemovedListener = 0;
		uint64_t m_destroyedListener		= 0;
	};
}
