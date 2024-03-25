
#pragma once

#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"

namespace LitchiEditor
{
	class ProjectHubPanel : public LitchiRuntime::PanelWindow
	{
	public:
		ProjectHubPanel(bool& p_readyToGo, std::string& p_path, std::string& p_projectName);

		void UpdateGoButton(const std::string& p_path);

		void CreateProject(const std::string& p_path);

		void RegisterProject(const std::string& p_path);

		void OpenProject(const std::string& p_path);

		void Draw() override;
	private:
		bool& m_readyToGo;
		std::string& m_path;
		std::string& m_projectName;
		LitchiRuntime::Button* m_goButton = nullptr;
	};
}
