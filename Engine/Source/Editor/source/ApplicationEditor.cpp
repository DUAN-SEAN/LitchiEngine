
#include "Runtime/Core/pch.h"
#include "Editor/include/ApplicationEditor.h"

#include <filesystem>
#include <easy/profiler.h>

#include "Editor/include/Panels/AssetBrowser.h"
#include "Editor/include/Panels/AssetView.h"
#include "Editor/include/Panels/Hierarchy.h"
#include "Editor/include/Panels/Inspector.h"
#include "Editor/include/Panels/MaterialEditor.h"
#include "Editor/include/Panels/MenuBar.h"
#include "Editor/include/Panels/SceneView.h"
#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"
#include <Runtime/Function/Framework/Component/UI/UIText.h>

#include "Editor/include/Panels/Console.h"
#include "Editor/include/Panels/GameView.h"
#include "Editor/include/Panels/Profiler.h"
#include "Editor/include/Panels/ProjectHubPanel.h"
#include "Editor/include/Panels/Toolbar.h"
#include "Runtime/Function/Framework/Component/Script/ScriptComponent.h"
#include "Runtime/Function/Framework/Component/Physcis/BoxCollider.h"
#include "Runtime/Function/Framework/Component/UI/RectTransform.h"
#include "Runtime/Function/Physics/physics.h"
#include "Runtime/Function/Renderer/Resource/Import/FontImporter.h"
#include "Runtime/Function/Renderer/Resource/Import/ImageImporter.h"

#include "Runtime/Resource/AssetManager.h"
#include "Runtime/Resource/FontManager.h"

#include "Runtime/Core/Tools/Utils/ConsoleHelper.h"
#include "Runtime/Function/Renderer/RHI/RHI_SwapChain.h"

LitchiEditor::ApplicationEditor* LitchiEditor::ApplicationEditor::instance_;

LitchiEditor::ApplicationEditor::ApplicationEditor() :m_canvas(), m_panelsManager(m_canvas), m_editorActions(m_panelsManager), ApplicationBase()
{

}

LitchiEditor::ApplicationEditor::~ApplicationEditor()
{
	uiManager = nullptr;

	delete m_rendererPath4SceneView;
	delete m_rendererPath4GameView;
	delete m_rendererPath4AssetView;

	modelManager->UnloadResources();
	modelManager = nullptr;
	prefabManager->UnloadResources();
	prefabManager = nullptr;
	shaderManager->UnloadResources();
	shaderManager = nullptr;
	materialManager->UnloadResources();
	materialManager = nullptr;
	fontManager->UnloadResources();
	fontManager = nullptr;
	textureManager->UnloadResources();
	textureManager = nullptr;

	sceneManager = nullptr;

	InputManager::UnInitialize();

	window = nullptr;

	Renderer::Shutdown();

	ImageImporter::Shutdown();
	FontImporter::Shutdown();
}

LitchiRuntime::LitchiApplicationType LitchiEditor::ApplicationEditor::GetApplicationType()
{
	return LitchiRuntime::LitchiApplicationType::Editor;
}

GameObject* CreateScriptObject(Scene* scene, std::string name, std::string scriptName)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto scriptComponent = go->AddComponent<ScriptComponent>();
	scriptComponent->SetClassName(scriptName);
	scriptComponent->PostResourceLoaded();

	return go;
}

bool LitchiEditor::ApplicationEditor::Initialize()
{
	instance_ = this;

	// Init Base
	if (!ApplicationBase::Initialize())
	{
		return false;
	}

	m_editorAssetsPath = PathParser::MakeNonWindowsStyle(std::filesystem::canonical("Data/Editor").string() + "/");

	// init uiManager
	uiManager = std::make_unique<UIManager>(window->GetGlfwWindow(), EStyle::DUNE_DARK);
	{
		uiManager->SetEditorLayoutSaveFilename(m_engineRootPath + "Config/layout.ini");
		uiManager->SetEditorLayoutAutosaveFrequency(60.0f);
		uiManager->EnableEditorLayoutSave(true);
		uiManager->EnableDocking(true);
	}
	ServiceLocator::Provide<UIManager>(*uiManager.get());

	if (!std::filesystem::exists(m_engineRootPath + "Config/layout.ini"))
		uiManager->ResetLayout(m_engineRootPath + "Config/layout.ini");

	DEBUG_LOG_INFO("ApplicationEditor Initialization End");

	RunProjectHub();

	return true;
}

void LitchiEditor::ApplicationEditor::Run()
{
	while (IsRunning())
	{
		EASY_BLOCK("Frame") {

			// PreUpdate
			window->PollEvents();

			// EASY_FUNCTION(profiler::colors::Magenta);
			Time::Update();
			InputManager::Tick();

			EASY_BLOCK("Update") {
				Update();
			}  EASY_END_BLOCK;

			if(!ApplicationBase::Instance()->window->IsMinimized())
			{
				EASY_BLOCK("Renderer") {
					Renderer::Tick();
				}  EASY_END_BLOCK;

				EASY_BLOCK("RenderViews") {
					RenderViews(Time::GetDeltaTime());
				}  EASY_END_BLOCK;

				EASY_BLOCK("RenderUI") {
					RenderUI();
				}  EASY_END_BLOCK;

			}

			// PostUpdate

			//window->SwapBuffers();
			InputManager::ClearEvents();
			++m_elapsedFrames;

		}  EASY_END_BLOCK;
	}
}

void LitchiEditor::ApplicationEditor::Update()
{
	auto scene = this->sceneManager->GetCurrentScene();
	if(scene)
	{
		if (auto editorMode = m_editorActions.GetCurrentEditorMode(); editorMode == EditorActions::EEditorMode::PLAY || editorMode == EditorActions::EEditorMode::FRAME_BY_FRAME)
		{
			auto& gameView = m_panelsManager.GetPanelAs<GameView>("Game View");
			if (!gameView.IsFocused() || !gameView.IsOpened())
			{
				return;
			}

			// Physics Tick
			m_restFixedTime += Time::GetDeltaTime();
			float fixedDeltaTime = Time::GetFixedUpdateTime();
			while (m_restFixedTime > fixedDeltaTime)
			{
				Physics::FixedUpdate(fixedDeltaTime);

				scene->FixedUpdate();

				m_restFixedTime -= fixedDeltaTime;
			}

			scene->Update();
			scene->LateUpdate();

			// Input::Update();
			//Audio::Update();

		}
		else
		{
			scene->OnEditorUpdate();
			// Edit Mode
		}
	}
	
	
	auto& profiler = m_panelsManager.GetPanelAs<Profiler>("Profiler");
	if(profiler.IsOpened())
	{
		profiler.Update(Time::GetDeltaTime());
	}
}

WindowSettings LitchiEditor::ApplicationEditor::CreateWindowSettings()
{
	WindowSettings windowSettings;
	windowSettings.title = "Litchi Editor";
	windowSettings.width = 1000;
	windowSettings.height = 580;
	windowSettings.minimumWidth = 1;
	windowSettings.minimumHeight = 1;
	windowSettings.maximized = true;
	return windowSettings;
}

void LitchiEditor::ApplicationEditor::OnSceneLoaded()
{
	const auto scene = sceneManager->GetCurrentScene();
	if(m_rendererPath4SceneView)
	{
		m_rendererPath4SceneView->SetScene(scene);
	}

	if(m_rendererPath4GameView)
	{
		m_rendererPath4GameView->SetScene(scene);
	}

	scene->Resolve();
	configManager->SetDefaultScenePath(sceneManager->GetCurrentSceneSourcePath());
}

bool LitchiEditor::ApplicationEditor::IsRunning() const
{
	return !window->ShouldClose();
}

void LitchiEditor::ApplicationEditor::RenderViews(float p_deltaTime)
{
	// EASY_FUNCTION(profiler::colors::Magenta);
	// ��ȾView 
	auto& sceneView = m_panelsManager.GetPanelAs<SceneView>("Scene View");
	if (sceneView.IsOpened())
	{
		sceneView.UpdateView(p_deltaTime);
		sceneView.Render();
	}

	auto& gameView = m_panelsManager.GetPanelAs<GameView>("Game View");
	if (gameView.IsOpened())
	{
		gameView.UpdateView(p_deltaTime);
		gameView.Render();
	}

	auto& assetView = m_panelsManager.GetPanelAs<AssetView>("Asset View");
	if (assetView.IsOpened())
	{
		assetView.UpdateView(p_deltaTime);
		assetView.Render();
	}
}

void LitchiEditor::ApplicationEditor::RenderUI()
{
	// EASY_FUNCTION(profiler::colors::Magenta);
	uiManager->Render();
}

void LitchiEditor::ApplicationEditor::SelectActor(GameObject* p_target)
{
	// debug
	if (p_target != nullptr)
	{
		auto name = p_target->GetName();
		auto transform = p_target->GetComponent<Transform>();
		auto position = transform->GetPosition();
		auto rotation = transform->GetRotation();
		auto rotationEuler = rotation.ToEulerAngles();
		DEBUG_LOG_INFO("SelectGO name:{},position:({},{},{}),rotation:({},{},{})", name, position.x, position.y, position.z, rotationEuler.x, rotationEuler.y, rotationEuler.z);
	}

	EDITOR_EXEC(SelectActor(p_target));
}

void LitchiEditor::ApplicationEditor::MoveToTarget(GameObject* p_target)
{
	auto name = p_target->GetName();
	DEBUG_LOG_INFO("MoveToTarget Target Name:{}", name);

	auto& sceneView = m_panelsManager.GetPanelAs<SceneView>("Scene View");
	sceneView.GetCameraController().MoveToTarget(p_target);
}

void LitchiEditor::ApplicationEditor::RunProjectHub()
{
	auto readyToGo = false;
	string path = "";
	string projectName = "";
	auto m_mainPanel = std::make_unique<ProjectHubPanel>(readyToGo, path, projectName);

	uiManager->SetCanvas(m_canvas);
	m_canvas.AddPanel(*m_mainPanel);
	m_canvas.MakeDockspace(true);

	auto oldSize = window->GetSize();
	auto oldPos = window->GetPosition();
	window->SetPosition(50.0f,50.0f);
	auto panelSize = m_mainPanel->GetSize();
	window->SetSize(static_cast<uint16_t>(panelSize.x), static_cast<uint16_t>(panelSize.y));
	Renderer::GetSwapChain()->ResizeToWindowSize();

	while (true)
	{
		if(!m_mainPanel->IsOpened())
		{
			break;
		}

		EASY_BLOCK("Frame") {
			// PreUpdate
			window->PollEvents();

			// EASY_FUNCTION(profiler::colors::Magenta);
			Time::Update();
			InputManager::Tick();

			if (!ApplicationBase::Instance()->window->IsMinimized())
			{
				EASY_BLOCK("Renderer") {
					Renderer::Tick();
				}  EASY_END_BLOCK;

				EASY_BLOCK("RenderUI") {
		
					RenderUI();
				}  EASY_END_BLOCK;
			}
			// PostUpdate

			//window->SwapBuffers();
			InputManager::ClearEvents();
			++m_elapsedFrames;
		}  EASY_END_BLOCK;

	}

	m_canvas.RemoveAllPanels();

	SetProjectPath(path);

	window->SetPosition(oldPos.first, oldPos.second);
	window->SetSize(oldSize.first, oldSize.second);

	OnProjectOpen();
}

void LitchiEditor::ApplicationEditor::OnProjectOpen()
{
	// ConsoleHelper::HideConsole();

	configManager = std::make_unique<ConfigManager>();
	if (!configManager->Initialize(m_projectPath))
	{
		DEBUG_LOG_ERROR("ConfigManager::Initialize Fail! ProjectPath:{}", m_projectPath);
	}

	auto projectAssetsPath = configManager->GetAssetFolderFullPath();

	FileSystem::SetAssetDirectoryPath(projectAssetsPath,m_engineAssetsPath);

	// Setup RendererPath
	SetupRendererPath();

	// Setup UI
	SetupEditorUI();

	// Load or Create Default Scene
	{
		auto& defaultScene = configManager->GetDefaultScenePath();
		if(!defaultScene.empty())
		{
			EDITOR_EXEC(LoadSceneFromDisk(defaultScene));
		}else
		{
			EDITOR_EXEC(LoadEmptyScene());
		}
	}
}

void LitchiEditor::ApplicationEditor::SetupEditorUI()
{
	PanelWindowSettings settings;
	settings.closable = true;
	settings.collapsable = true;
	settings.dockable = true;

	m_panelsManager.CreatePanel<MenuBar>("Menu Bar");
	m_panelsManager.CreatePanel<SceneView>("Scene View", true, settings, m_rendererPath4SceneView);
	m_panelsManager.CreatePanel<Hierarchy>("Hierarchy", true, settings);
	m_panelsManager.CreatePanel<Inspector>("Inspector", true, settings);
	m_panelsManager.CreatePanel<AssetBrowser>("Asset Browser", true, settings, GetEngineAssetsPath(), configManager->GetAssetFolderFullPath(), configManager->GetScriptFolderFullPath());
	m_panelsManager.CreatePanel<Profiler>("Profiler", true, settings, 0.25f);
	m_panelsManager.CreatePanel<Console>("Console", true, settings);
	m_panelsManager.CreatePanel<GameView>("Game View", true, settings, m_rendererPath4GameView);
	m_panelsManager.CreatePanel<AssetView>("Asset View", false, settings, m_rendererPath4AssetView);
	m_panelsManager.CreatePanel<Toolbar>("Toolbar", true, settings);
	m_panelsManager.CreatePanel<MaterialEditor>("Material Editor", false, settings);
	//m_panelsManager.CreatePanel<ProjectSettings>("Project Settings", false, settings);
	//m_panelsManager.CreatePanel<AssetProperties>("Asset Properties", false, settings);

	m_canvas.MakeDockspace(true);
	// 
	uiManager->SetCanvas(m_canvas);

}

void LitchiEditor::ApplicationEditor::SetupRendererPath()
{
	// prepare renderer path 
	if (m_rendererPath4SceneView == nullptr)
	{
		m_rendererPath4SceneView = new RendererPath(RendererPathType_SceneView);

		// update renderer path
		Renderer::UpdateRendererPath(RendererPathType_SceneView, m_rendererPath4SceneView);
		m_rendererPath4SceneView->SetActive(true);
	}

	if (m_rendererPath4GameView == nullptr)
	{
		m_rendererPath4GameView = new RendererPath(RendererPathType_GameView);

		// update renderer path
		Renderer::UpdateRendererPath(RendererPathType_GameView, m_rendererPath4GameView);
		// default active = false
	}

	if(m_rendererPath4AssetView == nullptr)
	{
		m_rendererPath4AssetView = new RendererPath(RendererPathType_AssetView);
		// update renderer path
		Renderer::UpdateRendererPath(RendererPathType_AssetView, m_rendererPath4AssetView);
	}

}
