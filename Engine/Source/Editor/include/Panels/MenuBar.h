
#pragma once
#include "Runtime/Function/UI/Panels/PanelMenuBar.h"
#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Widgets/Menu/MenuItem.h"

namespace LitchiEditor
{
	class MenuBar : public LitchiRuntime::PanelMenuBar
	{
		using PanelMap = std::unordered_map<std::string, std::pair<std::reference_wrapper<LitchiRuntime::PanelWindow>, std::reference_wrapper<LitchiRuntime::MenuItem>>>;

	public:
		/**
		* Constructor
		*/
		MenuBar();
		
		/**
		* Check inputs for menubar shortcuts
		* @param p_deltaTime
		*/
		void HandleShortcuts(float p_deltaTime);

		/**
		* Register a panel to the menu bar window menu
		*/
		void RegisterPanel(const std::string& p_name, LitchiRuntime::PanelWindow& p_panel);

	private:
		void CreateFileMenu();
		void CreateBuildMenu();
		void CreateWindowMenu();
		void CreateActorsMenu();
		void CreateResourcesMenu();
		void CreateSettingsMenu();
		void CreateLayoutMenu();
		void CreateHelpMenu();

		void UpdateToggleableItems();
		void OpenEveryWindows(bool p_state);

		void _Draw_Impl() override;

	private:
		PanelMap m_panels;

		LitchiRuntime::MenuList* m_windowMenu = nullptr;
	};
}
