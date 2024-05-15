
#include "Editor/include/Panels/ProjectSettings.h"
#include "Editor/include//Core/EditorActions.h"

#include <Runtime/Function/UI/Helpers/GUIDrawer.h>
#include <Runtime/Function/UI/Widgets/Layout/Columns.h>
#include <Runtime/Function/UI/Widgets/Layout/GroupCollapsable.h>
#include <Runtime/Function/UI/Widgets/Visual/Separator.h>
#include <Runtime/Function/UI/Widgets/Buttons/Button.h>

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/Rendering/Renderer_Definitions.h"
#define ENUM_TO_STRING(x) #x

using namespace LitchiRuntime;
namespace LitchiEditor
{

	ProjectSettings::ProjectSettings(const std::string& p_title, bool p_opened, const LitchiRuntime::PanelWindowSettings& p_windowSettings) :
		PanelWindow(p_title, p_opened, p_windowSettings),
		m_projectFile(ApplicationBase::Instance()->configManager->GetDataIniFile())
	{
		auto& saveButton = CreateWidget<LitchiRuntime::Button>("Apply");
		saveButton.idleBackgroundColor = { 0.0f, 0.5f, 0.0f };
		saveButton.ClickedEvent += [this]
			{
				ApplicationBase::Instance()->OnApplyProjectSettings();
				ApplicationBase::Instance()->configManager->Save();
				//m_projectFile.Rewrite();
			};

		saveButton.lineBreak = false;

		auto& resetButton = CreateWidget<Button>("Reset");
		resetButton.idleBackgroundColor = { 0.5f, 0.0f, 0.0f };
		resetButton.ClickedEvent += [this]
			{
				ApplicationBase::Instance()->configManager->ResetProjectSetting();
				ApplicationBase::Instance()->OnResetProjectSettings();
			};

		CreateWidget<Separator>();

		{
			/* Physics settings */
			auto& root = CreateWidget<GroupCollapsable>("Physics");
			auto& columns = root.CreateWidget<Columns<2>>();
			columns.widths[0] = 125;

			GUIDrawer::DrawScalar<float>(columns, "Gravity", GenerateGatherer<float>("gravity"), GenerateProvider<float>("gravity"), 0.1f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);
		}

		{
			/* Build settings */
			auto& generationRoot = CreateWidget<GroupCollapsable>("Build");
			auto& columns = generationRoot.CreateWidget<Columns<2>>();
			columns.widths[0] = 125;

			GUIDrawer::DrawBoolean(columns, "Development build", GenerateGatherer<bool>("dev_build"), GenerateProvider<bool>("dev_build"));
		}

		{
			/* Windowing settings */
			auto& windowingRoot = CreateWidget<GroupCollapsable>("Windowing");
			auto& columns = windowingRoot.CreateWidget<Columns<2>>();
			columns.widths[0] = 125;

			GUIDrawer::DrawScalar<int>(columns, "Resolution X", GenerateGatherer<int>("x_resolution"), GenerateProvider<int>("x_resolution"), 1, 0, 10000);
			GUIDrawer::DrawScalar<int>(columns, "Resolution Y", GenerateGatherer<int>("y_resolution"), GenerateProvider<int>("y_resolution"), 1, 0, 10000);
			GUIDrawer::DrawBoolean(columns, "Fullscreen", GenerateGatherer<bool>("fullscreen"), GenerateProvider<bool>("fullscreen"));
			GUIDrawer::DrawString(columns, "Executable name", GenerateGatherer<std::string>("executable_name"), GenerateProvider<std::string>("executable_name"));
		}

		{
			/* Rendering settings */
			auto& renderingRoot = CreateWidget<GroupCollapsable>("Rendering");
			auto& columns = renderingRoot.CreateWidget<Columns<2>>();
			columns.widths[0] = 125;

			GUIDrawer::DrawBoolean(columns, "Multi-sampling", GenerateGatherer<bool>("multisampling"), GenerateProvider<bool>("multisampling"));
			GUIDrawer::DrawScalar<int>(columns, "Samples", GenerateGatherer<int>("samples"), GenerateProvider<int>("samples"), 1, 2, 16);


			GUIDrawer::DrawBoolean(columns, "Hdr", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::Hdr)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::Hdr)));
			GUIDrawer::DrawBoolean(columns, "Show Mesh AABB", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::Aabb)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::Aabb)));
			GUIDrawer::DrawBoolean(columns, "Show Lights", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::Lights)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::Lights)));
			GUIDrawer::DrawBoolean(columns, "Show Grid", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::Grid)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::Grid)));
			GUIDrawer::DrawBoolean(columns, "Show Physics", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::Physics)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::Physics)));
			GUIDrawer::DrawBoolean(columns, "Show SelectionOutline", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::SelectionOutline)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::SelectionOutline)));
			GUIDrawer::DrawBoolean(columns, "Show TransformHandle", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::TransformHandle)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::TransformHandle)));
			GUIDrawer::DrawBoolean(columns, "Vsync", GenerateGatherer<bool>(ENUM_TO_STRING(Renderer_Option::Vsync)), GenerateProvider<bool>(ENUM_TO_STRING(Renderer_Option::Vsync)));
			GUIDrawer::DrawScalar<float>(columns, "ResolutionScale", GenerateGatherer<float>(ENUM_TO_STRING(Renderer_Option::ResolutionScale)), GenerateProvider<float>(ENUM_TO_STRING(Renderer_Option::ResolutionScale)));
			GUIDrawer::DrawScalar<float>(columns, "Anisotropy", GenerateGatherer<float>(ENUM_TO_STRING(Renderer_Option::Anisotropy)), GenerateProvider<float>(ENUM_TO_STRING(Renderer_Option::Anisotropy)));
			GUIDrawer::DrawScalar<float>(columns, "Exposure", GenerateGatherer<float>(ENUM_TO_STRING(Renderer_Option::Exposure)), GenerateProvider<float>(ENUM_TO_STRING(Renderer_Option::Exposure)));
			GUIDrawer::DrawScalar<float>(columns, "ShadowResolution", GenerateGatherer<float>(ENUM_TO_STRING(Renderer_Option::ShadowResolution)), GenerateProvider<float>(ENUM_TO_STRING(Renderer_Option::ShadowResolution)));
			GUIDrawer::DrawScalar<int>(columns, "Antialiasing", GenerateGatherer<int>(ENUM_TO_STRING(Renderer_Option::Antialiasing)), GenerateProvider<int>(ENUM_TO_STRING(Renderer_Option::Antialiasing)));
		}

		{
			/* Scene Management settings */
			auto& gameRoot = CreateWidget<GroupCollapsable>("Scene Management");
			auto& columns = gameRoot.CreateWidget<Columns<2>>();
			columns.widths[0] = 125;

			GUIDrawer::DrawDDString(columns, "Start scene", GenerateGatherer<std::string>("start_scene"), GenerateProvider<std::string>("start_scene"), "File");
		}
	}
}
