
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
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include <Runtime/Function/Framework/Component/UI/UIText.h>

#include "Editor/include/Panels/GameView.h"
#include "Editor/include/Panels/Toolbar.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Script/ScriptComponent.h"
#include "Runtime/Function/Framework/Component/Camera/Camera.h";
#include "Runtime/Function/Framework/Component/Physcis/BoxCollider.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidDynamic.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidStatic.h"
#include "Runtime/Function/Framework/Component/UI/RectTransform.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
#include "Runtime/Function/Physics/physics.h"
#include "Runtime/Function/Renderer/Resource/Import/FontImporter.h"

#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
#include "Runtime/Resource/AssetManager.h"
#include "Runtime/Resource/FontManager.h"

LitchiEditor::ApplicationEditor* LitchiEditor::ApplicationEditor::instance_;
struct data
{
	union
	{
		float float_value;
		unsigned char byte_value[4];
	};
};
LitchiEditor::ApplicationEditor::ApplicationEditor() :m_canvas(), m_panelsManager(m_canvas), m_editorActions(m_panelsManager), ApplicationBase()
{

}

LitchiEditor::ApplicationEditor::~ApplicationEditor()
{
	/*if (ImGui::GetCurrentContext())
	{
		ImGui::RHI::shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}*/

	// 析构其他
	sceneManager = nullptr;
	modelManager->UnloadResources();
	modelManager = nullptr;
	shaderManager->UnloadResources();
	shaderManager = nullptr;
	materialManager->UnloadResources();
	materialManager = nullptr;
	fontManager->UnloadResources();
	fontManager = nullptr;
	textureManager->UnloadResources();
	textureManager = nullptr;
	window = nullptr;
	ResourceCache::Shutdown();
	// World::Shutdown();
	Renderer::Shutdown();
	// Physics::Shutdown();
	// ThreadPool::Shutdown();
	// Event::Shutdown();
	// Audio::Shutdown();
	// Profiler::Shutdown();
	//ImageImporterExporter::Shutdown();
	FontImporter::Shutdown();

	delete m_rendererPath4SceneView;
	delete m_rendererPath4GameView;
}

GameObject* CreateModel(Scene* scene, std::string name, Vector3 position, Quaternion rotation, Vector3 scale, std::string modelPath)
{
	auto gameObject4Cube = scene->CreateGameObject("Cube");
	auto transform4Cube = gameObject4Cube->GetComponent<Transform>();
	transform4Cube->SetPositionLocal(position);
	transform4Cube->SetRotationLocal(rotation);
	transform4Cube->SetScaleLocal(scale);
	auto meshFilter4Cube = gameObject4Cube->AddComponent<MeshFilter>();
	auto meshRenderer4Cube = gameObject4Cube->AddComponent<MeshRenderer>();
	auto mesh = ApplicationBase::Instance()->modelManager->LoadResource(modelPath);
	meshFilter4Cube->SetGeometry(mesh);
	meshRenderer4Cube->SetDefaultMaterial();

	return gameObject4Cube;
}

GameObject* CreateSkinnedModel(Scene* scene, std::string name, Vector3 position, Quaternion rotation, Vector3 scale, std::string materialPath, std::string modelPath)
{
	auto gameObject4Cube = scene->CreateGameObject(name);
	auto transform4Cube = gameObject4Cube->GetComponent<Transform>();
	transform4Cube->SetPositionLocal(position);
	transform4Cube->SetRotationLocal(rotation);
	transform4Cube->SetScaleLocal(scale);

	auto meshFilter4Cube = gameObject4Cube->AddComponent<MeshFilter>();
	auto meshRenderer4Cube = gameObject4Cube->AddComponent<SkinnedMeshRenderer>();
	auto animator = gameObject4Cube->AddComponent<Animator>();

	auto material = ApplicationBase::Instance()->materialManager->LoadResource(materialPath);

	auto mesh = ApplicationBase::Instance()->modelManager->LoadResource(modelPath);
	meshFilter4Cube->SetGeometry(mesh);
	meshRenderer4Cube->SetMaterial(material);

	// 初始化animator
	std::unordered_map<std::string, AnimationClip> animations;
	mesh->GetAnimations(animations);
	auto firstClipName = animations.begin()->first;
	animator->SetAnimationClipMap(animations);
	animator->Play(firstClipName);


	return gameObject4Cube;
}

GameObject* CreateLightObject(Scene* scene, std::string name, Vector3 pos, Quaternion rotation)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto transform = go->GetComponent<Transform>();
	transform->SetPositionLocal(Vector3(pos.x, pos.y, pos.z));
	transform->SetRotationLocal(Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	transform->PostResourceLoaded();

	auto directionalLight = go->AddComponent<Light>();
	directionalLight->SetColor({ 0.3f,0.6f,0.7f });
	directionalLight->SetIntensity(LightIntensity::bulb_100_watt);

	return go;
}

GameObject* CreateUIImageObject(Scene* scene, std::string name, Vector3 pos, Quaternion rotation, RHI_Texture* image)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto transform = go->AddComponent<Transform>();
	transform->SetPositionLocal(Vector3(pos.x, pos.y, pos.z));
	transform->SetRotationLocal(Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	transform->SetScaleLocal(Vector3(1));
	transform->PostResourceLoaded();

	/*auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->model_path = "";
	mesh_filter->PostResourceLoaded();

	auto mesh_renderer = go->AddComponent<MeshRenderer>();
	mesh_renderer->material_path = "Engine\\Materials\\UIImage.mat";
	mesh_renderer->PostResourceLoaded();*/
	go->SetLayer(0x02); // UI 层级为2

	auto uiImage = go->AddComponent<UIImage>();
	uiImage->SetTexture(image);

	return go;
}

GameObject* CreateUITextObject(Scene* scene, std::string name, Vector3 pos, Quaternion rotation, Font* font)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto transform = go->AddComponent<Transform>();
	transform->SetPositionLocal(Vector3(pos.x, pos.y, pos.z));
	transform->SetRotationLocal(Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	transform->SetScaleLocal(Vector3(1));
	transform->PostResourceLoaded();

	/*auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->model_path = "";
	mesh_filter->PostResourceLoaded();

	auto mesh_renderer = go->AddComponent<MeshRenderer>();
	mesh_renderer->material_path = "Engine\\Materials\\UIText.mat";
	mesh_renderer->PostResourceLoaded();*/
	go->SetLayer(0x02); // UI 层级为2

	// 创建UIText
	auto uiText = go->AddComponent<UIText>();
	uiText->SetFont(font);
	uiText->SetText("EF");
	uiText->SetColor(Color::White);

	return go;
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

void LitchiEditor::ApplicationEditor::Init()
{
	instance_ = this;

	// Init Base
	ApplicationBase::Init();

	// init uiManager
	uiManager = std::make_unique<UIManager>(window->GetGlfwWindow(), EStyle::DUNE_DARK);
	{
		uiManager->SetEditorLayoutSaveFilename(this->projectAssetsPath + "Config\\layout.ini");
		uiManager->SetEditorLayoutAutosaveFrequency(60.0f);
		uiManager->EnableEditorLayoutSave(true);
		uiManager->EnableDocking(true);
	}
	ServiceLocator::Provide<UIManager>(*uiManager.get());

	if (!std::filesystem::exists(this->projectAssetsPath + "Config\\layout.ini"))
		uiManager->ResetLayout(this->projectAssetsPath + "Config\\layout.ini");

	// Setup RendererPath
	SetupRendererPath();

	// Setup UI
	SetupUI();

	/* below code is test process */

	// test 1
	{
		auto font = fontManager->LoadResource("Engine\\Fonts\\Ruda-Bold.ttf");
		font->AddText("Hello World", Vector2::Zero);

		sceneManager->LoadEmptyScene();
		auto* scene = sceneManager->GetCurrentScene();
		// AssetManager::LoadAsset( projectAssetsPath + "Scenes\\New Scene3.scene", scene);

		// create go from prefab
		//auto mesh = modelManager->LoadResource("Engine\\Models\\Catwalk Walk Forward HighKnees.fbx");
		//auto mesh_prefab = mesh->GetModelPrefab();
		//auto instantiateGo = scene->InstantiatePrefab(mesh_prefab,nullptr);

		// create camera
		EDITOR_EXEC(CreateMonoComponentActor<Camera>(false, nullptr));

		// create cube
		EDITOR_EXEC(CreateActorWithModel("Engine\\Models\\Cube.fbx", true, nullptr, "Cube"));

		CreateLightObject(scene, "Directional Light", Vector3::Zero, Quaternion::FromEulerAngles(42, 0, 0));

		auto canvas = EDITOR_EXEC(CreateMonoComponentActor<UICanvas>());
		canvas->SetName("Canvas");
		auto text = EDITOR_EXEC(CreateUIActor<UIText>(true, canvas));
		text->SetName("Text");
		text->GetComponent<UIText>()->SetFontPath("Engine\\Fonts\\Calibri.ttf");
		text->GetComponent<UIText>()->SetText("Hello World !");
		text->GetComponent<UIText>()->PostResourceModify();

		auto image = EDITOR_EXEC(CreateUIActor<UIImage>(true, canvas));
		image->SetName("Image");
		image->GetComponent<UIImage>()->SetImagePath("Engine\\Textures\\liuyifei.png");
		image->GetComponent<UIImage>()->PostResourceModify();
		image->GetComponent<RectTransform>()->SetPos({ 960, 540,0.0f });
		image->GetComponent<RectTransform>()->SetSize({ 500.0f, 500.0f });

		scene->Resolve();
		m_rendererPath4SceneView->SetScene(sceneManager->GetCurrentScene());
		m_rendererPath4GameView->SetScene(sceneManager->GetCurrentScene());
	}

	//// test 2
	//{
	//	sceneManager->LoadScene("Scenes\\New Scene4.scene", false);
	//	sceneManager->GetCurrentScene()->Resolve();

	//	m_rendererPath4SceneView->SetScene(sceneManager->GetCurrentScene());
	//	m_rendererPath4GameView->SetScene(sceneManager->GetCurrentScene());
	//}
}

void LitchiEditor::ApplicationEditor::Run()
{
	while (IsRunning())
	{
		EASY_BLOCK("Frame") {
			// PreUpdate
			window->PollEvents();

			EASY_BLOCK("Update") {
				Update();
			}  EASY_END_BLOCK;

			// Update
			// 检测是否删除物体
			// 检测运行模式 Game or no
			// 渲染Views

			EASY_BLOCK("RenderViews") {
				RenderViews(Time::delta_time());
			}  EASY_END_BLOCK;

			EASY_BLOCK("Renderer") {
				Renderer::Tick();
			}  EASY_END_BLOCK;


			EASY_BLOCK("RenderUI") {
				// 渲染UI
				RenderUI();
			}  EASY_END_BLOCK;

			// PostUpdate

			//window->SwapBuffers();
			InputManager::ClearEvents();
			++m_elapsedFrames;
		}  EASY_END_BLOCK;
	}
}

void LitchiEditor::ApplicationEditor::Update()
{
	// EASY_FUNCTION(profiler::colors::Magenta);
	Time::Update();
	UpdateScreenSize();
	InputManager::Tick();

	if (auto editorMode = m_editorActions.GetCurrentEditorMode(); editorMode == EditorActions::EEditorMode::PLAY || editorMode == EditorActions::EEditorMode::FRAME_BY_FRAME)
	{
		auto scene = this->sceneManager->GetCurrentScene();

		// Physics Tick
		m_restFixedTime += Time::delta_time();
		float fixedDeltaTime = Time::fixed_update_time();
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
		auto scene = this->sceneManager->GetCurrentScene();

		scene->OnEditorUpdate();
		// Edit Mode
	}
}

bool LitchiEditor::ApplicationEditor::IsRunning() const
{
	return !window->ShouldClose();
}

void LitchiEditor::ApplicationEditor::RenderViews(float p_deltaTime)
{
	// EASY_FUNCTION(profiler::colors::Magenta);
	// 渲染View 
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

	/*auto& assetView = m_panelsManager.GetPanelAs<AssetView>("Asset View");
	if (assetView.IsOpened())
	{
		simulatedLightSSBO->Bind(0);

		assetView.Update(p_deltaTime);
		assetView.Render();
		simulatedLightSSBO->Unbind();
	}*/
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

	// todo Inspector 选择
	EDITOR_EXEC(SelectActor(p_target));
}

void LitchiEditor::ApplicationEditor::MoveToTarget(GameObject* p_target)
{
	auto name = p_target->GetName();
	DEBUG_LOG_INFO("MoveToTarget Target Name:{}", name);

	auto& sceneView = m_panelsManager.GetPanelAs<SceneView>("Scene View");
	sceneView.GetCameraController().MoveToTarget(p_target);
}

void LitchiEditor::ApplicationEditor::SetupUI()
{
	PanelWindowSettings settings;
	settings.closable = true;
	settings.collapsable = true;
	settings.dockable = true;

	m_panelsManager.CreatePanel<MenuBar>("Menu Bar");
	m_panelsManager.CreatePanel<SceneView>("Scene View", true, settings, m_rendererPath4SceneView);
	m_panelsManager.CreatePanel<Hierarchy>("Hierarchy", true, settings);
	m_panelsManager.CreatePanel<Inspector>("Inspector", true, settings);
	m_panelsManager.CreatePanel<AssetBrowser>("Asset Browser", true, settings, projectAssetsPath);
	//m_panelsManager.CreatePanel<Profiler>("Profiler", true, settings, 0.25f);
	//m_panelsManager.CreatePanel<Console>("Console", true, settings);
	m_panelsManager.CreatePanel<GameView>("Game View", true, settings, m_rendererPath4GameView);
	m_panelsManager.CreatePanel<AssetView>("Asset View", false, settings, m_rendererPath4SceneView);
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

}
