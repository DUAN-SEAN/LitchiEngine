
#include <filesystem>
#include <fstream>
#include <Editor/include/Panels/ProjectHubPanel.h>

#include "Editor/include/Panels/AViewControllable.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"
#include "Runtime/Core/Window/Dialogs/MessageBox.h"
#include "Runtime/Core/Window/Dialogs/OpenFileDialog.h"
#include "Runtime/Core/Window/Dialogs/SaveFileDialog.h"
#include "Runtime/Function/UI/Widgets/InputFields/InputText.h"
#include "Runtime/Function/UI/Widgets/Layout/Columns.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include "Runtime/Function/UI/Widgets/Layout/Spacing.h"
#include "Runtime/Function/UI/Widgets/Texts/Text.h"
#include "Runtime/Function/UI/Widgets/Visual/Separator.h"


#define PROJECTS_FILE std::string(std::string(getenv("APPDATA")) + "\\LitchiEngine\\LitchiEditor\\projects.ini")

using namespace LitchiRuntime;
namespace LitchiEditor
{
	ProjectHubPanel::ProjectHubPanel(bool& p_readyToGo, std::string& p_path, std::string& p_projectName) :
		PanelWindow("Litchi - Project Hub", true),
		m_readyToGo(p_readyToGo),
		m_path(p_path),
		m_projectName(p_projectName)
	{
		resizable = false;
		movable = false;
		titleBar = false;

		std::filesystem::create_directories(std::string(getenv("APPDATA")) + "\\LitchiEngine\\LitchiEditor\\");

		SetSize({ 1000, 580 });
		SetPosition({ 0.f, 0.f });

		auto& openProjectButton = CreateWidget<LitchiRuntime::Button>("Open Project");
		auto& newProjectButton = CreateWidget<Button>("New Project");
		auto& pathField = CreateWidget<InputText>("");
		m_goButton = &CreateWidget<Button>("GO");

		pathField.ContentChangedEvent += [this, &pathField](std::string p_content)
			{
				pathField.content = PathParser::MakeWindowsStyle(p_content);

				if (pathField.content != "" && pathField.content.back() != '\\')
					pathField.content += '\\';

				UpdateGoButton(pathField.content);
			};

		UpdateGoButton("");

		openProjectButton.idleBackgroundColor = { 0.7f, 0.5f, 0.f };
		newProjectButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };

		openProjectButton.ClickedEvent += [this]
			{
				OpenFileDialog dialog("Open project");
				dialog.AddFileType("Litchi Project", "*.ovproject");
				dialog.Show();

				std::string ovProjectPath = dialog.GetSelectedFilePath();
				std::string rootFolderPath = PathParser::GetContainingFolder(ovProjectPath);

				if (dialog.HasSucceeded())
				{
					RegisterProject(rootFolderPath);
					OpenProject(rootFolderPath);
				}
			};

		newProjectButton.ClickedEvent += [this, &pathField]
			{
				SaveFileDialog dialog("New project location");
				dialog.DefineExtension("Litchi Project", "..");
				dialog.Show();
				if (dialog.HasSucceeded())
				{
					std::string result = dialog.GetSelectedFilePath();
					pathField.content = std::string(result.data(), result.data() + result.size() - std::string("..").size()); // remove auto extension
					pathField.content += "\\";
					UpdateGoButton(pathField.content);
				}
			};

		m_goButton->ClickedEvent += [this, &pathField]
			{
				CreateProject(pathField.content);
				RegisterProject(pathField.content);
				OpenProject(pathField.content);
			};

		openProjectButton.lineBreak = false;
		newProjectButton.lineBreak = false;
		pathField.lineBreak = false;

		for (uint8_t i = 0; i < 4; ++i)
			CreateWidget<Spacing>();

		CreateWidget<Separator>();

		for (uint8_t i = 0; i < 4; ++i)
			CreateWidget<Spacing>();

		auto& columns = CreateWidget<Columns<2>>();

		columns.widths = { 750, 500 };

		std::string line;
		std::ifstream myfile(PROJECTS_FILE);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				if (std::filesystem::exists(line)) // TODO: Delete line from the file
				{
					auto& text = columns.CreateWidget<Text>(line);
					auto& actions = columns.CreateWidget<Group>();
					auto& openButton = actions.CreateWidget<Button>("Open");
					auto& deleteButton = actions.CreateWidget<Button>("Delete");

					openButton.idleBackgroundColor = { 0.7f, 0.5f, 0.f };
					deleteButton.idleBackgroundColor = { 0.5f, 0.f, 0.f };

					openButton.ClickedEvent += [this, line]
						{
							OpenProject(line);
						};

					std::string toErase = line;
					deleteButton.ClickedEvent += [this, &text, &actions, toErase]
						{
							text.Destroy();
							actions.Destroy();

							std::string line;
							std::ifstream fin(PROJECTS_FILE);
							std::ofstream temp("temp");

							while (getline(fin, line))
								if (line != toErase)
									temp << line << std::endl;

							temp.close();
							fin.close();

							std::filesystem::remove(PROJECTS_FILE);
							std::filesystem::rename("temp", PROJECTS_FILE);
						};

					openButton.lineBreak = false;
					deleteButton.lineBreak;
				}
			}
			myfile.close();
		}
	}

	void ProjectHubPanel::UpdateGoButton(const std::string& p_path)
	{
		bool validPath = p_path != "";
		m_goButton->idleBackgroundColor = validPath ? Color{ 0.f, 0.5f, 0.0f } : Color{ 0.1f, 0.1f, 0.1f };
		m_goButton->disabled = !validPath;
	}

	void ProjectHubPanel::CreateProject(const std::string& p_path)
	{
		if (!std::filesystem::exists(p_path))
		{
			std::filesystem::create_directory(p_path);
			std::filesystem::create_directory(p_path + "Assets\\");
			std::filesystem::create_directory(p_path + "Scripts\\");
			std::ofstream projectFile(p_path + '\\' + PathParser::GetElementName(std::string(p_path.data(), p_path.data() + p_path.size() - 1)) + ".litchiProject");
		}
	}

	void ProjectHubPanel::RegisterProject(const std::string& p_path)
	{
		bool pathAlreadyRegistered = false;

		{
			std::string line;
			std::ifstream myfile(PROJECTS_FILE);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					if (line == p_path)
					{
						pathAlreadyRegistered = true;
						break;
					}
				}
				myfile.close();
			}
		}

		if (!pathAlreadyRegistered)
		{
			std::ofstream projectsFile(PROJECTS_FILE, std::ios::app);
			projectsFile << p_path << std::endl;
		}
	}

	void ProjectHubPanel::OpenProject(const std::string& p_path)
	{
		m_readyToGo = std::filesystem::exists(p_path);
		if (!m_readyToGo)
		{
			MessageBox errorMessage("Project not found", "The selected project does not exists", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
		}
		else
		{
			m_path = p_path;
			m_projectName = PathParser::GetElementName(m_path);
			Close();
		}
	}

	void ProjectHubPanel::Draw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 50.f, 50.f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

		PanelWindow::Draw();

		ImGui::PopStyleVar(2);
	}
}
