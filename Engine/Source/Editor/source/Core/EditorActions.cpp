
#include <filesystem>
#include <iostream>
#include <fstream>

#include "Runtime/Core/Log/debug.h"

#include "Editor/include/ApplicationEditor.h"
#include "Editor/include/Panels/Inspector.h"
#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Core/Tools/Utils/String.h"

#include "Editor/include/Core/EditorActions.h"

#include "Editor/include/Panels/Hierarchy.h"
#include "Editor/include/Panels/SceneView.h"
#include "Runtime/Core/Window/Dialogs/MessageBox.h"
#include "Runtime/Core/Window/Dialogs/OpenFileDialog.h"
#include "Runtime/Core/Window/Dialogs/SaveFileDialog.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Renderer/Resources/Loaders/MaterialLoader.h"
#include "Runtime/Function/Renderer/Resources/Loaders/ShaderLoader.h"
#include "Runtime/Resource/asset_manager.h"

LitchiEditor::EditorActions::EditorActions(PanelsManager& p_panelsManager) :
	m_panelsManager(p_panelsManager)
{
	LitchiRuntime::ServiceLocator::Provide<LitchiEditor::EditorActions>(*this);

	/*LitchiEditor::ApplicationEditor::Instance()->sceneManager.CurrentSceneSourcePathChangedEvent += [this](const std::string& p_newPath)
	{
		std::string titleExtra = " - " + (p_newPath.empty() ? "Untitled Scene" : GetResourcePath(p_newPath));
		LitchiEditor::ApplicationEditor::Instance()->window->SetTitle(LitchiEditor::ApplicationEditor::Instance()->windowSettings.title + titleExtra);
	};*/
}

void LitchiEditor::EditorActions::LoadEmptyScene()
{
	if (GetCurrentEditorMode() != EEditorMode::EDIT)
		StopPlaying();

	LitchiEditor::ApplicationEditor::Instance()->sceneManager->CreateScene("Empty Scene");
	m_panelsManager.GetPanelAs<LitchiEditor::Hierarchy>("Hierarchy").Refresh();

	DEBUG_LOG_INFO("New scene created");
}

void LitchiEditor::EditorActions::SaveCurrentSceneTo(const std::string& p_path)
{
	// 获取当前的场景
	LitchiEditor::ApplicationEditor::Instance()->sceneManager->SaveCurrentScene(p_path);
}

void LitchiEditor::EditorActions::LoadSceneFromDisk(const std::string& p_path, bool p_absolute)
{
	if (GetCurrentEditorMode() != EEditorMode::EDIT)
		StopPlaying();

	LitchiEditor::ApplicationEditor::Instance()->sceneManager->LoadScene(p_path);

	m_panelsManager.GetPanelAs<LitchiEditor::SceneView>("Scene View").Focus();
	m_panelsManager.GetPanelAs<LitchiEditor::Hierarchy>("Hierarchy").Refresh();

	DEBUG_LOG_INFO("Scene loaded from disk: " + LitchiEditor::ApplicationEditor::Instance()->sceneManager->GetCurrentSceneSourcePath());
}

bool LitchiEditor::EditorActions::IsCurrentSceneLoadedFromDisk() const
{
	return LitchiEditor::ApplicationEditor::Instance()->sceneManager->IsCurrentSceneLoadedFromPath();
}

void LitchiEditor::EditorActions::SaveSceneChanges()
{
	if (IsCurrentSceneLoadedFromDisk())
	{
		SaveCurrentSceneTo(LitchiEditor::ApplicationEditor::Instance()->sceneManager->GetCurrentSceneSourcePath());
		DEBUG_LOG_INFO("Current scene saved to: " + LitchiEditor::ApplicationEditor::Instance()->sceneManager->GetCurrentSceneSourcePath());
	}
	else
	{
		SaveAs();
	}
}

void LitchiEditor::EditorActions::SaveAs()
{
	SaveFileDialog dialog("New Scene");
	dialog.SetInitialDirectory(LitchiEditor::ApplicationEditor::Instance()->projectAssetsPath + "New Scene");
	dialog.DefineExtension("Litchi Scene", ".scene");
	dialog.Show();

	if (dialog.HasSucceeded())
	{
		if (dialog.IsFileExisting())
		{
			MessageBox message("File already exists!", "The file \"" + dialog.GetSelectedFileName() + "\" already exists.\n\nUsing this file as the new home for your scene will erase any content stored in this file.\n\nAre you ok with that?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::YES_NO, true);
			switch (message.GetUserAction())
			{
			case MessageBox::EUserAction::YES: break;
			case MessageBox::EUserAction::NO: return;
			}
		}

		SaveCurrentSceneTo(dialog.GetSelectedFilePath());
		DEBUG_LOG_INFO("Current scene saved to: " + dialog.GetSelectedFilePath());
	}
}

void LitchiEditor::EditorActions::RefreshScripts()
{
	//LitchiEditor::ApplicationEditor::Instance()->scriptInterpreter->RefreshAll();
	//m_panelsManager.GetPanelAs<Inspector>("Inspector").Refresh();
	//if (LitchiEditor::ApplicationEditor::Instance()->scriptInterpreter->IsOk())
	//	DEBUG_LOG_INFO("Scripts interpretation succeeded!");
}

std::optional<std::string> LitchiEditor::EditorActions::SelectBuildFolder()
{
	//OvWindowing::Dialogs::SaveFileDialog dialog("Build location");
	//dialog.DefineExtension("Game Build", "..");
	//dialog.Show();
	//if (dialog.HasSucceeded())
	//{
	//	std::string result = dialog.GetSelectedFilePath();
	//	result = std::string(result.data(), result.data() + result.size() - std::string("..").size()) + "\\"; // remove auto extension
	//	if (!std::filesystem::exists(result))
	//		return result;
	//	else
	//	{
	//		OvWindowing::Dialogs::MessageBox message("Folder already exists!", "The folder \"" + result + "\" already exists.\n\nPlease select another location and try again", OvWindowing::Dialogs::MessageBox::EMessageType::WARNING, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
	//		return {};
	//	}
	//}
	//else
	//{
	//	return {};
	//}

	return {};
}

void LitchiEditor::EditorActions::Build(bool p_autoRun, bool p_tempFolder)
{
	//std::string destinationFolder;

	//if (p_tempFolder)
	//{
	//	destinationFolder = std::string(getenv("APPDATA")) + "\\OverloadTech\\OvEditor\\TempBuild\\";
	//	try
	//	{
	//		std::filesystem::remove_all(destinationFolder);
	//	}
	//	catch (std::filesystem::filesystem_error error)
	//	{
	//		OvWindowing::Dialogs::MessageBox message("Temporary build failed", "The temporary folder is currently being used by another process", OvWindowing::Dialogs::MessageBox::EMessageType::ERROR, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
	//		return;
	//	}
	//}
	//else if (auto res = SelectBuildFolder(); res.has_value())
	//	destinationFolder = res.value();
	//else
	//	return; // Operation cancelled (No folder selected)

	//BuildAtLocation(LitchiEditor::ApplicationEditor::Instance()->projectSettings.Get<bool>("dev_build") ? "Development" : "Shipping", destinationFolder, p_autoRun);
}

void LitchiEditor::EditorActions::BuildAtLocation(const std::string & p_configuration, const std::string p_buildPath, bool p_autoRun)
{
	/*std::string buildPath(p_buildPath);
	std::string executableName = LitchiEditor::ApplicationEditor::Instance()->projectSettings.Get<std::string>("executable_name") + ".exe";

	bool failed = false;

	DEBUG_LOG_INFO("Preparing to build at location: \"" + buildPath + "\"");

	std::filesystem::remove_all(buildPath);

	if (std::filesystem::create_directory(buildPath))
	{
		DEBUG_LOG_INFO("Build directory created");

		if (std::filesystem::create_directory(buildPath + "Data\\"))
		{
			DEBUG_LOG_INFO("Data directory created");

			if (std::filesystem::create_directory(buildPath + "Data\\User\\"))
			{
				DEBUG_LOG_INFO("Data\\User directory created");

				std::error_code err;

				std::filesystem::copy(LitchiEditor::ApplicationEditor::Instance()->projectFilePath, buildPath + "Data\\User\\Game.ini", err);

				if (!err)
				{
					DEBUG_LOG_INFO("Data\\User\\Game.ini file generated");
		
					std::filesystem::copy(LitchiEditor::ApplicationEditor::Instance()->projectAssetsPath, buildPath + "Data\\User\\Assets\\", std::filesystem::copy_options::recursive, err);

					if (!std::filesystem::exists(buildPath + "Data\\User\\Assets\\" + (LitchiEditor::ApplicationEditor::Instance()->projectSettings.Get<std::string>("start_scene"))))
					{
						OVLOG_ERROR("Failed to find Start Scene at expected path. Verify your Project Setings.");
						OvWindowing::Dialogs::MessageBox message("Build Failure", "An error occured during the building of your game.\nCheck the console for more information", OvWindowing::Dialogs::MessageBox::EMessageType::ERROR, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
						std::filesystem::remove_all(buildPath);
						return;						
					}

					if (!err)
					{
						DEBUG_LOG_INFO("Data\\User\\Assets\\ directory copied");

						std::filesystem::copy(LitchiEditor::ApplicationEditor::Instance()->projectScriptsPath, buildPath + "Data\\User\\Scripts\\", std::filesystem::copy_options::recursive, err);

						if (!err)
						{
							DEBUG_LOG_INFO("Data\\User\\Scripts\\ directory copied");

							std::filesystem::copy(LitchiEditor::ApplicationEditor::Instance()->engineAssetsPath, buildPath + "Data\\Engine\\", std::filesystem::copy_options::recursive, err);

							if (!err)
							{
								DEBUG_LOG_INFO("Data\\Engine\\ directory copied");
							}
							else
							{
								OVLOG_ERROR("Data\\Engine\\ directory failed to copy");
								failed = true;
							}
						}
						else
						{
							OVLOG_ERROR("Data\\User\\Scripts\\ directory failed to copy");
							failed = true;
						}
					}
					else
					{
						OVLOG_ERROR("Data\\User\\Assets\\ directory failed to copy");
						failed = true;
					}
				}
				else
				{
					OVLOG_ERROR("Data\\User\\Game.ini file failed to generate");
					failed = true;
				}

				std::string builderFolder = "Builder\\" + p_configuration + "\\";

				if (std::filesystem::exists(builderFolder))
				{
					std::error_code err;

					std::filesystem::copy(builderFolder, buildPath, err);

					if (!err)
					{
						DEBUG_LOG_INFO("Builder data (Dlls and executatble) copied");

						std::filesystem::rename(buildPath + "OvGame.exe", buildPath + executableName, err);

						if (!err)
						{
							DEBUG_LOG_INFO("Game executable renamed to " + executableName);

							if (p_autoRun)
							{
								std::string exePath = buildPath + executableName;
								DEBUG_LOG_INFO("Launching the game at location: \"" + exePath + "\"");
								if (std::filesystem::exists(exePath))
									SystemCalls::OpenFile(exePath, buildPath);
								else
								{
									OVLOG_ERROR("Failed to start the game: Executable not found");
									failed = true;
								}
							}
						}
						else
						{
							OVLOG_ERROR("Game executable failed to rename");
							failed = true;
						}
					}
					else
					{
						OVLOG_ERROR("Builder data (Dlls and executatble) failed to copy");
						failed = true;
					}
				}
				else
				{
					const std::string buildConfiguration = p_configuration == "Development" ? "Debug" : "Release";
					OVLOG_ERROR("Builder folder for \"" + p_configuration + "\" not found. Verify you have compiled Engine source code in '" + buildConfiguration + "' configuration.");
					failed = true;
				}
			}
		}
	}
	else
	{
		OVLOG_ERROR("Build directory failed to create");
		failed = true;
	}

	if (failed)
	{
		std::filesystem::remove_all(buildPath);
		OvWindowing::Dialogs::MessageBox message("Build Failure", "An error occured during the building of your game.\nCheck the console for more information", OvWindowing::Dialogs::MessageBox::EMessageType::ERROR, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
	}*/
}

void LitchiEditor::EditorActions::DelayAction(std::function<void()> p_action, uint32_t p_frames)
{
	m_delayedActions.emplace_back(p_frames + 1, p_action);
}

void LitchiEditor::EditorActions::ExecuteDelayedActions()
{
	std::for_each(m_delayedActions.begin(), m_delayedActions.end(), [](std::pair<uint32_t, std::function<void()>> & p_element)
	{
		--p_element.first;

		if (p_element.first == 0)
			p_element.second();
	});

	m_delayedActions.erase(std::remove_if(m_delayedActions.begin(), m_delayedActions.end(), [](std::pair<uint32_t, std::function<void()>> & p_element)
	{
		return p_element.first == 0;
	}), m_delayedActions.end());
}
//
//LitchiEditor::Context& LitchiEditor::EditorActions::GetContext()
//{
//	return m_context;
//}
//
//LitchiEditor::EditorRenderer & LitchiEditor::EditorActions::GetRenderer()
//{
//	return m_renderer;
//}

LitchiEditor::PanelsManager& LitchiEditor::EditorActions::GetPanelsManager()
{
	return m_panelsManager;
}

void LitchiEditor::EditorActions::SetActorSpawnAtOrigin(bool p_value)
{
	if (p_value)
		m_actorSpawnMode = EActorSpawnMode::ORIGIN;
	else
		m_actorSpawnMode = EActorSpawnMode::FRONT;
}

void LitchiEditor::EditorActions::SetActorSpawnMode(EActorSpawnMode p_value)
{
	m_actorSpawnMode = p_value;
}

void LitchiEditor::EditorActions::ResetLayout()
{
    // DelayAction([this]() {LitchiEditor::ApplicationEditor::Instance()->uiManager->ResetLayout("Config\\layout.ini"); });
}

void LitchiEditor::EditorActions::SetSceneViewCameraSpeed(int p_speed)
{
	// EDITOR_PANEL(SceneView, "Scene View").GetCameraController().SetSpeed((float)p_speed);
}

int LitchiEditor::EditorActions::GetSceneViewCameraSpeed()
{
	// return (int)EDITOR_PANEL(SceneView, "Scene View").GetCameraController().GetSpeed();
	return 0;
}

void LitchiEditor::EditorActions::SetAssetViewCameraSpeed(int p_speed)
{
	// EDITOR_PANEL(AssetView, "Asset View").GetCameraController().SetSpeed((float)p_speed);
}

int LitchiEditor::EditorActions::GetAssetViewCameraSpeed()
{
	// return (int)EDITOR_PANEL(AssetView, "Asset View").GetCameraController().GetSpeed();
	return 1;
}

void LitchiEditor::EditorActions::ResetSceneViewCameraPosition()
{
	// EDITOR_PANEL(SceneView, "Scene View").GetCameraController().SetPosition({ -10.0f, 4.0f, 10.0f });
	// TODO
	// EDITOR_PANEL(SceneView, "Scene View").GetCamera().SetPitch(-10.0f);
	// EDITOR_PANEL(SceneView, "Scene View").GetCamera().SetYaw(-45.0f);
	// EDITOR_PANEL(SceneView, "Scene View").GetCamera().SetRoll(0.0f);
}

void LitchiEditor::EditorActions::ResetAssetViewCameraPosition()
{
	// EDITOR_PANEL(AssetView, "Asset View").GetCameraController().SetPosition({ -10.0f, 4.0f, 10.0f });
	// TODO
	// EDITOR_PANEL(AssetView, "Asset View").GetCamera().SetPitch(-10.0f);
	// EDITOR_PANEL(AssetView, "Asset View").GetCamera().SetYaw(-45.0f);
	// EDITOR_PANEL(AssetView, "Asset View").GetCamera().SetRoll(0.0f);
}

LitchiEditor::EditorActions::EEditorMode LitchiEditor::EditorActions::GetCurrentEditorMode() const
{
	return m_editorMode;
}

void LitchiEditor::EditorActions::SetEditorMode(EEditorMode p_newEditorMode)
{
	m_editorMode = p_newEditorMode;
	EditorModeChangedEvent.Invoke(m_editorMode);
}

void LitchiEditor::EditorActions::StartPlaying()
{
	/*if (m_editorMode == EEditorMode::EDIT)
	{
		LitchiEditor::ApplicationEditor::Instance()->scriptInterpreter->RefreshAll();
		EDITOR_PANEL(Inspector, "Inspector").Refresh();

		if (LitchiEditor::ApplicationEditor::Instance()->scriptInterpreter->IsOk())
		{
			PlayEvent.Invoke();
			m_sceneBackup.Clear();
			tinyxml2::XMLNode* node = m_sceneBackup.NewElement("root");
			m_sceneBackup.InsertFirstChild(node);
			LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentScene()->OnSerialize(m_sceneBackup, node);
			m_panelsManager.GetPanelAs<LitchiEditor::GameView>("Game View").Focus();
			LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentScene()->Play();
			SetEditorMode(EEditorMode::PLAY);
		}
	}
	else
	{
		LitchiEditor::ApplicationEditor::Instance()->audioEngine->Unsuspend();
		SetEditorMode(EEditorMode::PLAY);
	}*/
}

void LitchiEditor::EditorActions::PauseGame()
{
	/*SetEditorMode(EEditorMode::PAUSE);
	LitchiEditor::ApplicationEditor::Instance()->audioEngine->Suspend();*/
}

void LitchiEditor::EditorActions::StopPlaying()
{
	//if (m_editorMode != EEditorMode::EDIT)
	//{
	//	ImGui::GetIO().DisableMouseUpdate = false;
	//	LitchiEditor::ApplicationEditor::Instance()->window->SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
	//	SetEditorMode(EEditorMode::EDIT);
	//	bool loadedFromDisk = LitchiEditor::ApplicationEditor::Instance()->sceneManager.IsCurrentSceneLoadedFromDisk();
	//	std::string sceneSourcePath = LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentSceneSourcePath();

	//	int64_t focusedActorID = -1;

	//	if (auto targetActor = EDITOR_PANEL(Inspector, "Inspector").GetTargetActor())
	//		focusedActorID = targetActor->GetID();

	//	LitchiEditor::ApplicationEditor::Instance()->sceneManager.LoadSceneFromMemory(m_sceneBackup);
	//	if (loadedFromDisk)
	//		LitchiEditor::ApplicationEditor::Instance()->sceneManager.StoreCurrentSceneSourcePath(sceneSourcePath); // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
	//	m_sceneBackup.Clear();
	//	EDITOR_PANEL(SceneView, "Scene View").Focus();
	//	if (auto actorInstance = LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentScene()->FindActorByID(focusedActorID))
	//		EDITOR_PANEL(Inspector, "Inspector").FocusActor(*actorInstance);
	//}
}

void LitchiEditor::EditorActions::NextFrame()
{
	if (m_editorMode == EEditorMode::PLAY || m_editorMode == EEditorMode::PAUSE)
		SetEditorMode(EEditorMode::FRAME_BY_FRAME);
}

glm::vec3 LitchiEditor::EditorActions::CalculateActorSpawnPoint(float p_distanceToCamera)
{
	auto& sceneView = m_panelsManager.GetPanelAs<LitchiEditor::SceneView>("Scene View");
	return sceneView.GetCameraPosition() + sceneView.GetCameraRotation() * glm::vec3(0,0,-1) * p_distanceToCamera;
}

LitchiRuntime::GameObject* LitchiEditor::EditorActions::CreateEmptyActor(bool p_focusOnCreation, LitchiRuntime::GameObject* p_parent, const std::string& p_name)
{
    const auto currentScene = LitchiEditor::ApplicationEditor::Instance()->sceneManager->GetCurrentScene();
	auto* instance = currentScene->CreateGameObject(p_name.empty() ? "Empty Object" : p_name);
	auto* transform = instance->AddComponent<Transform>();

	if (p_parent)
		instance->SetParent(p_parent);

	if (m_actorSpawnMode == EActorSpawnMode::FRONT)
		transform->SetLocalPosition(CalculateActorSpawnPoint(10.0f));

	if (p_focusOnCreation)
		SelectActor(instance);

	ApplicationEditor::Instance()->m_panelsManager.GetPanelAs<Hierarchy>("Hierarchy").AddActorByInstance(instance);

	DEBUG_LOG_INFO("Actor created");

	return instance;
}

LitchiRuntime::GameObject* LitchiEditor::EditorActions::CreateActorWithModel(const std::string& p_path, bool p_focusOnCreation, LitchiRuntime::GameObject* p_parent, const std::string& p_name)
{
	auto instance = CreateEmptyActor(false, p_parent, p_name);

	auto meshRenderer = instance->AddComponent<MeshRenderer>();
	auto meshFilter = instance->AddComponent<MeshFilter>();
	
	meshFilter->modelPath = p_path;
	meshFilter->meshIndex = 0;
	meshFilter->PostResourceLoaded();

	meshRenderer->materialPath = "Engine\\Materials\\Default.mat";
	meshRenderer->PostResourceLoaded();
	
	if (p_focusOnCreation)
		SelectActor(instance);

	return instance;
}

bool LitchiEditor::EditorActions::DestroyActor(LitchiRuntime::GameObject* p_actor)
{
	/*p_actor.MarkAsDestroy();
	DEBUG_LOG_INFO("Actor destroyed");*/
	return true;
}

std::string FindDuplicatedActorUniqueName(LitchiRuntime::GameObject& p_duplicated, LitchiRuntime::GameObject& p_newActor, LitchiRuntime::Scene& p_scene)
{
   /* const auto parent = p_newActor.GetParent();
    const auto adjacentActors = parent ? parent->GetChildren() : p_scene.GetActors();

    auto availabilityChecker = [&parent, &adjacentActors](std::string target) -> bool
    {
        const auto isActorNameTaken = [&target, parent](auto actor) { return (parent || !actor->GetParent()) && actor->GetName() == target; };
        return std::find_if(adjacentActors.begin(), adjacentActors.end(), isActorNameTaken) == adjacentActors.end();
    };

    return String::GenerateUnique(p_duplicated.GetName(), availabilityChecker);*/
	return "";
}

void LitchiEditor::EditorActions::DuplicateActor(LitchiRuntime::GameObject* p_toDuplicate, LitchiRuntime::GameObject* p_forcedParent, bool p_focus)
{
	/*tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* actorsRoot = doc.NewElement("actors");
	p_toDuplicate.OnSerialize(doc, actorsRoot);
	auto& newActor = CreateEmptyActor(false);
	int64_t idToUse = newActor.GetID();
	tinyxml2::XMLElement* currentActor = actorsRoot->FirstChildElement("actor");
	newActor.OnDeserialize(doc, currentActor);
	
	newActor.SetID(idToUse);

	if (p_forcedParent)
		newActor.SetParent(*p_forcedParent);
	else
	{
        auto currentScene = LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentScene();

        if (newActor.GetParentID() > 0)
        {
            if (auto found = currentScene->FindActorByID(newActor.GetParentID()); found)
            {
                newActor.SetParent(*found);
            }
        }

        const auto uniqueName = FindDuplicatedActorUniqueName(p_toDuplicate, newActor, *currentScene);
        newActor.SetName(uniqueName);
	}

	if (p_focus)
		SelectActor(newActor);

	for (auto& child : p_toDuplicate.GetChildren())
		DuplicateActor(*child, &newActor, false);*/
}

void LitchiEditor::EditorActions::SelectActor(LitchiRuntime::GameObject* p_target)
{
	EDITOR_PANEL(Inspector, "Inspector").FocusActor(p_target);
}

void LitchiEditor::EditorActions::UnselectActor()
{
	EDITOR_PANEL(Inspector, "Inspector").UnFocus();
}

bool LitchiEditor::EditorActions::IsAnyActorSelected() const
{
	return EDITOR_PANEL(Inspector, "Inspector").GetTargetActor();
}

LitchiRuntime::GameObject* LitchiEditor::EditorActions::GetSelectedActor() const
{
	return EDITOR_PANEL(Inspector, "Inspector").GetTargetActor();
}

void LitchiEditor::EditorActions::MoveToTarget(LitchiRuntime::GameObject* p_target)
{
	EDITOR_PANEL(SceneView, "Scene View").GetCameraController().MoveToTarget(p_target);
}

void LitchiEditor::EditorActions::CompileShaders()
{
	for (auto shader : LitchiEditor::ApplicationEditor::Instance()->shaderManager->GetResources())
		LitchiRuntime::Loaders::ShaderLoader::Recompile(*shader.second, GetRealPath(shader.second->path));
}

void LitchiEditor::EditorActions::SaveMaterials()
{
	for (auto& [id, material] : LitchiEditor::ApplicationEditor::Instance()->materialManager->GetResources())
		Loaders::MaterialLoader::Save(*material, GetRealPath(material->path));
}

bool LitchiEditor::EditorActions::ImportAsset(const std::string& p_initialDestinationDirectory)
{
	std::string modelFormats = "*.fbx;*.obj;";
	std::string textureFormats = "*.png;*.jpeg;*.jpg;*.tga";
	std::string shaderFormats = "*.glsl;";
	std::string soundFormats = "*.mp3;*.ogg;*.wav;";

	OpenFileDialog selectAssetDialog("Select an asset to import");
	selectAssetDialog.AddFileType("Any supported format", modelFormats + textureFormats + shaderFormats + soundFormats);
	selectAssetDialog.AddFileType("Model (.fbx, .obj)", modelFormats);
	selectAssetDialog.AddFileType("Texture (.png, .jpeg, .jpg, .tga)", textureFormats);
	selectAssetDialog.AddFileType("Shader (.glsl)", shaderFormats);
	selectAssetDialog.AddFileType("Sound (.mp3, .ogg, .wav)", soundFormats);
	selectAssetDialog.Show();

	if (selectAssetDialog.HasSucceeded())
	{
		std::string source = selectAssetDialog.GetSelectedFilePath();
		std::string extension = '.' + PathParser::GetExtension(source);
		std::string filename = selectAssetDialog.GetSelectedFileName();

		SaveFileDialog saveLocationDialog("Where to import?");
		saveLocationDialog.SetInitialDirectory(p_initialDestinationDirectory + filename);
		saveLocationDialog.DefineExtension(extension, extension);
		saveLocationDialog.Show();

		if (saveLocationDialog.HasSucceeded())
		{
			std::string destination = saveLocationDialog.GetSelectedFilePath();

			if (!std::filesystem::exists(destination) || LitchiRuntime::MessageBox("File already exists", "The destination you have selected already exists, importing this file will erase the previous file content, are you sure about that?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::OK_CANCEL).GetUserAction() == MessageBox::EUserAction::OK)
			{
				std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
				DEBUG_LOG_INFO("Asset \"" + destination + "\" imported");
				return true;
			}
		}
	}

	return false;
}

bool LitchiEditor::EditorActions::ImportAssetAtLocation(const std::string& p_destination)
{
	std::string modelFormats = "*.fbx;*.obj;";
	std::string textureFormats = "*.png;*.jpeg;*.jpg;*.tga;";
	std::string shaderFormats = "*.glsl;";
	std::string soundFormats = "*.mp3;*.ogg;*.wav;";

	OpenFileDialog selectAssetDialog("Select an asset to import");
	selectAssetDialog.AddFileType("Any supported format", modelFormats + textureFormats + shaderFormats + soundFormats);
	selectAssetDialog.AddFileType("Model (.fbx, .obj)", modelFormats);
	selectAssetDialog.AddFileType("Texture (.png, .jpeg, .jpg, .tga)", textureFormats);
	selectAssetDialog.AddFileType("Shader (.glsl)", shaderFormats);
	selectAssetDialog.AddFileType("Sound (.mp3, .ogg, .wav)", soundFormats);
	selectAssetDialog.Show();

	if (selectAssetDialog.HasSucceeded())
	{
		std::string source = selectAssetDialog.GetSelectedFilePath();
		std::string destination = p_destination + selectAssetDialog.GetSelectedFileName();

		if (!std::filesystem::exists(destination) || MessageBox("File already exists", "The destination you have selected already exists, importing this file will erase the previous file content, are you sure about that?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::OK_CANCEL).GetUserAction() == MessageBox::EUserAction::OK)
		{
			std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
			DEBUG_LOG_INFO("Asset \"" + destination + "\" imported");
			return true;
		}
	}

	return false;
}

// Duplicate from AResourceManager.h
std::string LitchiEditor::EditorActions::GetRealPath(const std::string & p_path)
{
	std::string result;

	if (p_path[0] == ':') // The path is an engine path
	{
		result = LitchiEditor::ApplicationEditor::Instance()->engineAssetsPath + std::string(p_path.data() + 1, p_path.data() + p_path.size());
	}
	else // The path is a project path
	{
		result = LitchiEditor::ApplicationEditor::Instance()->projectAssetsPath + p_path;
	}

	return result;
}

std::string LitchiEditor::EditorActions::GetResourcePath(const std::string & p_path, bool p_isFromEngine)
{
	std::string result = p_path;

	if (LitchiRuntime::String::Replace(result, p_isFromEngine ? LitchiEditor::ApplicationEditor::Instance()->engineAssetsPath : LitchiEditor::ApplicationEditor::Instance()->projectAssetsPath, ""))
	{
		if (p_isFromEngine)
			result = ':' + result;
	}

	return result;
}

std::string LitchiEditor::EditorActions::GetScriptPath(const std::string & p_path)
{
	std::string result = p_path;

	String::Replace(result, LitchiEditor::ApplicationEditor::Instance()->projectScriptsPath, "");
	String::Replace(result, ".lua", "");

	return result;
}

void LitchiEditor::EditorActions::PropagateFolderRename(std::string p_previousName, std::string p_newName)
{
	p_previousName = PathParser::MakeNonWindowsStyle(p_previousName);
	p_newName = PathParser::MakeNonWindowsStyle(p_newName);

	for (auto& p : std::filesystem::recursive_directory_iterator(p_newName))
	{
		if (!p.is_directory())
		{
			std::string newFileName = PathParser::MakeNonWindowsStyle(p.path().string());
			std::string previousFileName;

			for (char c : newFileName)
			{
				previousFileName += c;
				if (previousFileName == p_newName)
					previousFileName = p_previousName;
			}

			PropagateFileRename(PathParser::MakeWindowsStyle(previousFileName), PathParser::MakeWindowsStyle(newFileName));
		}
	}
}

void LitchiEditor::EditorActions::PropagateFolderDestruction(std::string p_folderPath)
{
	for (auto& p : std::filesystem::recursive_directory_iterator(p_folderPath))
	{
		if (!p.is_directory())
		{
			PropagateFileRename(PathParser::MakeWindowsStyle(p.path().string()), "?");
		}
	}
}

void LitchiEditor::EditorActions::PropagateScriptRename(std::string p_previousName, std::string p_newName)
{
	/*p_previousName = GetScriptPath(p_previousName);
	p_newName = GetScriptPath(p_newName);

	if (auto currentScene = LitchiEditor::ApplicationEditor::Instance()->sceneManager->GetCurrentScene())
		for (auto actor : currentScene->GetActors())
			if (actor->RemoveBehaviour(p_previousName))
				actor->AddBehaviour(p_newName);

	PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, PathParser::EFileType::SCENE);

	EDITOR_PANEL(Inspector, "Inspector").Refresh();*/
}

void LitchiEditor::EditorActions::PropagateFileRename(std::string p_previousName, std::string p_newName)
{
	p_previousName = GetResourcePath(p_previousName);
	p_newName = GetResourcePath(p_newName);

	if (p_newName != "?")
	{
		/* If not a real rename is asked (Not delete) */

		if (LitchiRuntime::ServiceLocator::Get<ModelManager>().MoveResource(p_previousName, p_newName))
		{
			LitchiRuntime::Model* resource = LitchiRuntime::ServiceLocator::Get<ModelManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(LitchiRuntime::Model, path)) = p_newName;
		}

		if (LitchiRuntime::ServiceLocator::Get<TextureManager>().MoveResource(p_previousName, p_newName))
		{
			LitchiRuntime::Texture* resource = LitchiRuntime::ServiceLocator::Get<TextureManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(LitchiRuntime::Texture, path)) = p_newName;
		}

		if (LitchiRuntime::ServiceLocator::Get<ShaderManager>().MoveResource(p_previousName, p_newName))
		{
			LitchiRuntime::Resource::Shader* resource = LitchiRuntime::ServiceLocator::Get<ShaderManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(LitchiRuntime::Resource::Shader, path)) = p_newName;
		}

		if (LitchiRuntime::ServiceLocator::Get<MaterialManager>().MoveResource(p_previousName, p_newName))
		{
			Resource::Material* resource = LitchiRuntime::ServiceLocator::Get<MaterialManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(Resource::Material, path)) = p_newName;
		}

		/*if (LitchiRuntime::ServiceLocator::Get<SoundManager>().MoveResource(p_previousName, p_newName))
		{
			OvAudio::Sound* resource = LitchiRuntime::ServiceLocator::Get<SoundManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(OvAudio::Sound, path)) = p_newName;
		}*/
	}
	else
	{
		/*if (auto texture = LitchiRuntime::ServiceLocator::Get<TextureManager>().GetResource(p_previousName, false))
		{
			for (auto[name, instance] : LitchiRuntime::ServiceLocator::Get<MaterialManager>().GetResources())
				if (instance)
					for (auto&[name, value] : instance->GetUniformsData())
						if (value.has_value() && value.type() == typeid(LitchiRuntime::Texture*))
							if (std::any_cast<LitchiRuntime::Texture*>(value) == texture)
								value = static_cast<LitchiRuntime::Texture*>(nullptr);

			auto& assetView = EDITOR_PANEL(AssetView, "Asset View");
			auto assetViewRes = assetView.GetResource();
			if (auto pval = std::get_if<LitchiRuntime::Texture*>(&assetViewRes); pval && *pval)
				assetView.SetResource(static_cast<LitchiRuntime::Texture*>(nullptr));

			LitchiRuntime::ServiceLocator::Get<TextureManager>().UnloadResource(p_previousName);
		}
		
		if (auto shader = LitchiRuntime::ServiceLocator::Get<ShaderManager>().GetResource(p_previousName, false))
		{
			auto& materialEditor = EDITOR_PANEL(MaterialEditor, "Material Editor");

			for (auto[name, instance] : LitchiRuntime::ServiceLocator::Get<MaterialManager>().GetResources())
				if (instance && instance->GetShader() == shader)
					instance->SetShader(nullptr);

			LitchiRuntime::ServiceLocator::Get<ShaderManager>().UnloadResource(p_previousName);
		}

		if (auto model = LitchiRuntime::ServiceLocator::Get<ModelManager>().GetResource(p_previousName, false))
		{
			auto& assetView = EDITOR_PANEL(AssetView, "Asset View");
			auto assetViewRes = assetView.GetResource();
			if (auto pval = std::get_if<LitchiRuntime::Model*>(&assetViewRes); pval && *pval)
				assetView.SetResource(static_cast<LitchiRuntime::Model*>(nullptr));

			if (auto currentScene = LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentScene())
				for (auto actor : currentScene->GetActors())
					if (auto modelRenderer = actor->GetComponent<MeshFilter>(); modelRenderer && modelRenderer->GetModel() == model)
						modelRenderer->SetModel(nullptr);

			LitchiRuntime::ServiceLocator::Get<ModelManager>().UnloadResource(p_previousName);
		}

		if (auto material = LitchiRuntime::ServiceLocator::Get<MaterialManager>().GetResource(p_previousName, false))
		{
			auto& materialEditor = EDITOR_PANEL(MaterialEditor, "Material Editor");
			
			if (materialEditor.GetTarget() == material)
				materialEditor.RemoveTarget();

			auto& assetView = EDITOR_PANEL(AssetView, "Asset View");
			auto assetViewRes = assetView.GetResource();
			if (auto pval = std::get_if<Resource::Material*>(&assetViewRes); pval && *pval)
				assetView.SetResource(static_cast<Resource::Material*>(nullptr));

			if (auto currentScene = LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentScene())
				for (auto actor : currentScene->GetActors())
					if (auto materialRenderer = actor->GetComponent<OvCore::ECS::Components::CMaterialRenderer>(); materialRenderer)
						materialRenderer->RemoveMaterialByInstance(*material);

			LitchiRuntime::ServiceLocator::Get<MaterialManager>().UnloadResource(p_previousName);
		}

		if (auto sound = LitchiRuntime::ServiceLocator::Get<SoundManager>().GetResource(p_previousName, false))
		{
			if (auto currentScene = LitchiEditor::ApplicationEditor::Instance()->sceneManager.GetCurrentScene())
				for (auto actor : currentScene->GetActors())
					if (auto audioSource = actor->GetComponent<OvCore::ECS::Components::CAudioSource>(); audioSource && audioSource->GetSound() == sound)
						audioSource->SetSound(nullptr);

			LitchiRuntime::ServiceLocator::Get<SoundManager>().UnloadResource(p_previousName);
		}*/
	}

	switch (PathParser::GetFileType(p_previousName))
	{
	case PathParser::EFileType::MATERIAL:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, PathParser::EFileType::SCENE);
		break;
	case PathParser::EFileType::MODEL:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, PathParser::EFileType::SCENE);
		break;
	case PathParser::EFileType::SHADER:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, PathParser::EFileType::MATERIAL);
		break;
	case PathParser::EFileType::TEXTURE:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, PathParser::EFileType::MATERIAL);
		break;
	case PathParser::EFileType::SOUND:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, PathParser::EFileType::SCENE);
		break;
	}

	ApplicationEditor::Instance()->m_panelsManager.GetPanelAs<Inspector>("Inspector").Refresh();
	// EDITOR_PANEL(MaterialEditor, "Material Editor").Refresh();
}

void LitchiEditor::EditorActions::PropagateFileRenameThroughSavedFilesOfType(const std::string& p_previousName, const std::string& p_newName, LitchiRuntime::PathParser::EFileType p_fileType)
{
	for (auto& entry : std::filesystem::recursive_directory_iterator(LitchiEditor::ApplicationEditor::Instance()->projectAssetsPath))
	{
		if (PathParser::GetFileType(entry.path().string()) == p_fileType)
		{
			using namespace std;

			{
				ifstream in(entry.path().string().c_str());
				ofstream out("TEMP");
				string wordToReplace(">" + p_previousName + "<");
				string wordToReplaceWith(">" + p_newName + "<");

				string line;
				size_t len = wordToReplace.length();
				while (getline(in, line))
				{
					if (String::Replace(line, wordToReplace, wordToReplaceWith))
						DEBUG_LOG_INFO("Asset retargeting: \"" + p_previousName + "\" to \"" + p_newName + "\" in \"" + entry.path().string() + "\"");
					out << line << '\n';
				}

				out.close(); in.close();
			}

			std::filesystem::copy_file("TEMP", entry.path(), std::filesystem::copy_options::overwrite_existing);
			std::filesystem::remove("TEMP");
		}
	}
}
