
#include "Editor/include/ApplicationEditor.h"

#include <filesystem>

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
#include "Runtime/Function/Renderer/Resources/Loaders/ModelLoader.h"
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include <Runtime/Function/Framework/Component/UI/UIText.h>

#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Script/ScriptComponent.h"
#include "Runtime/Function/Framework/Component/Camera/Camera.h";

#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
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
LitchiEditor::ApplicationEditor::ApplicationEditor() :m_canvas(), m_panelsManager(m_canvas), m_editorActions(m_panelsManager),ApplicationBase()
{
	
}

LitchiEditor::ApplicationEditor::~ApplicationEditor()
{
}

GameObject* CreateCube(Scene* scene, std::string name, Vector3 position, Quaternion rotation, Vector3 scale)
{
	auto gameObject4Cube = scene->CreateGameObject("Cube");
	auto transform4Cube = gameObject4Cube->GetComponent<Transform>();
	transform4Cube->SetPositionLocal(position);
	transform4Cube->SetRotationLocal(rotation);
	transform4Cube->SetScaleLocal(scale);
	auto meshFilter4Cube = gameObject4Cube->AddComponent<MeshFilter>();
	auto meshRenderer4Cube = gameObject4Cube->AddComponent<MeshRenderer>();
	meshFilter4Cube->SetGeometry(Renderer::GetStandardMesh(Renderer_MeshType::Cube).get());
	meshRenderer4Cube->SetDefaultMaterial();

	return gameObject4Cube;
}

GameObject* CreateDefaultObject(Scene* scene, std::string name, std::string modelPath, std::string materialPath, float y, float z)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto transform = go->AddComponent<Transform>();
	transform->SetPositionLocal((0.0f, y, z));
	transform->PostResourceLoaded();

	auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->modelPath = modelPath;
	mesh_filter->PostResourceLoaded();

	auto mesh_renderer = go->AddComponent<MeshRenderer>();
	mesh_renderer->materialPath = materialPath;
	mesh_renderer->PostResourceLoaded();
	go->SetLayer(0x01);

	return go;
}

GameObject* CreateDefaultObject4Skinned(Scene* scene, std::string name, std::string modelPath, std::string materialPath, float y, float z)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto transform = go->AddComponent<Transform>();
	transform->SetPositionLocal({ 0.0, y, z });
	transform->PostResourceLoaded();

	auto animator = go->AddComponent<Animator>();

	auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->modelPath = modelPath;
	mesh_filter->PostResourceLoaded();

	auto mesh_renderer = go->AddComponent<SkinnedMeshRenderer>();
	mesh_renderer->materialPath = materialPath;
	mesh_renderer->PostResourceLoaded();
	go->SetLayer(0x01);

	// 初始化animator todo:
	/*auto* model = mesh_filter->GetModel();
	std::unordered_map<std::string, AnimationClip> animations;
	model->GetAnimations(animations);
	auto firstClipName = animations.begin()->first;
	animator->SetAnimationClipMap(animations);
	animator->Play(firstClipName);*/

	return go;
}

GameObject* CreateDefaultObject(Scene* scene, std::string name, std::string modelPath, std::string materialPath, Vector3 position, Quaternion rotation, Vector3 scale)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto transform = go->AddComponent<Transform>();
	transform->SetPositionLocal(Vector3(position.x, position.y, position.z));
	transform->SetRotationLocal(Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	transform->SetScaleLocal(Vector3(scale.x, scale.y, scale.z));
	transform->PostResourceLoaded();

	auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->modelPath = modelPath;
	mesh_filter->PostResourceLoaded();

	auto mesh_renderer = go->AddComponent<MeshRenderer>();
	mesh_renderer->materialPath = materialPath;
	mesh_renderer->PostResourceLoaded();
	go->SetLayer(0x01);

	return go;
}

GameObject* CreateLightObject(Scene* scene, std::string name, Vector3 pos, Quaternion rotation)
{
	GameObject* go = scene->CreateGameObject(name);
	go->PostResourceLoaded();

	auto transform = go->AddComponent<Transform>();
	transform->SetPositionLocal(Vector3(pos.x, pos.y, pos.z));
	transform->SetRotationLocal(Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	transform->PostResourceLoaded();

	auto directionalLight = go->AddComponent<Light>();
	directionalLight->SetColor((0.3, 0.6, 0.7));
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
	uiText->SetColor(Vector4(1));

	return go;
}

GameObject* CreateScriptObject(Scene* scene,std::string name,std::string scriptName)
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

	ApplicationBase::Init();
	
	// 初始化InputManager
	uiManager = std::make_unique<UIManager>(window->GetGlfwWindow(), EStyle::DUNE_DARK);
	{
		/*uiManager->LoadFont("Ruda_Big", editorAssetsPath + "\\Fonts\\Ruda-Bold.ttf", 16);
		uiManager->LoadFont("Ruda_Small", editorAssetsPath + "\\Fonts\\Ruda-Bold.ttf", 12);
		uiManager->LoadFont("Ruda_Medium", editorAssetsPath + "\\Fonts\\Ruda-Bold.ttf", 14);
		uiManager->UseFont("Ruda_Medium");*/
		// uiManager->SetEditorLayoutSaveFilename(std::string(getenv("APPDATA")) + "\\LitchiEngine\\Editor\\layout.ini");
		uiManager->SetEditorLayoutSaveFilename(this->projectAssetsPath + "Config\\layout.ini");
		uiManager->SetEditorLayoutAutosaveFrequency(60.0f);
		uiManager->EnableEditorLayoutSave(true);
		uiManager->EnableDocking(true);
	}

	if (!std::filesystem::exists(this->projectAssetsPath + "Config\\layout.ini"))
		uiManager->ResetLayout(this->projectAssetsPath +"Config\\layout.ini");


	// 初始化Cameraf
	m_renderCamera4SceneView = new RenderCamera();
	m_renderCamera4SceneView->Initialize();

	// prepare renderer path 
	if (m_rendererPath4SceneView == nullptr)
	{
		m_rendererPath4SceneView = new RendererPath(RendererPathType_SceneView, m_renderCamera4SceneView);
		
		// update renderer path
		Renderer::UpdateRendererPath(RendererPathType_SceneView, m_rendererPath4SceneView);
	}

	auto scene = sceneManager->CreateScene("Default");
	m_rendererPath4SceneView->SetScene(ApplicationBase::Instance()->sceneManager->GetCurrentScene());

	//Vector3& camera_position = Vector3(0.0f, 0.0f, -10.0f);
	//Vector3& camera_rotation = Vector3(0.0f, 0.0, 0.0f);
	//auto camerGo = scene->CreateGameObject("Camera");
	//auto camera = camerGo->AddComponent<Camera>();
	//auto transform4Camera = camerGo->GetComponent<Transform>();
	//transform4Camera->SetPositionLocal(camera_position); // place it at the top of the capsule
	//transform4Camera->SetRotation(Quaternion::FromEulerAngles(camera_rotation));

	CreateCube(scene, "Cube01", Vector3(0.0f, 4.0f, 0.0f), Quaternion::Identity, Vector3::One);

	CreateCube(scene, "Cube02", Vector3(4.0f, 0.0f, 0.0f), Quaternion::Identity, Vector3::One);


	auto textMat= materialManager->LoadResource("Engine\\Materials\\Test.mat");

	// auto cube= CreateCube(scene, "Cube02", Vector3(0.0f, 0.0f, 4.0f), Quaternion::Identity, Vector3::One);


	//Mesh* mesh = new Mesh();
	//mesh->LoadFromFile(projectAssetsPath+"Engine\\Models\\Cube.fbx");
	/*mesh = new Mesh();
	mesh->LoadFromFile(projectAssetsPath + "Engine\\Models\\LadyCat.fbx");*/

	// Setup UI
	SetupUI();

	for (auto go : scene->GetAllGameObjectList())
	{
		m_panelsManager.GetPanelAs<Hierarchy>("Hierarchy").AddActorByInstance(go);
	}

	m_panelsManager.GetPanelAs<Hierarchy>("Hierarchy").Refresh();


	ServiceLocator::Provide(*shaderManager.get());
	ServiceLocator::Provide(*modelManager.get());
	ServiceLocator::Provide(*materialManager.get());
	ServiceLocator::Provide(*textureManager.get());
	ServiceLocator::Provide(*fontManager.get());
}

void LitchiEditor::ApplicationEditor::Run()
{
	while (IsRunning())
	{
		// PreUpdate
		window->PollEvents();

		Update();

		// Update
		// 检测是否删除物体
		// 检测运行模式 Game or no
		// 渲染Views
		RenderViews(Time::delta_time());

		Renderer::Tick();

		// 渲染UI
		RenderUI();

		// PostUpdate

		//window->SwapBuffers();
		InputManager::ClearEvents();
		++m_elapsedFrames;
	}
}

void LitchiEditor::ApplicationEditor::Update()
{
	Time::Update();
	UpdateScreenSize();

	InputManager::Tick();

	auto scene = this->sceneManager->GetCurrentScene();

	// Update
	/*for (auto* entity : scene->GetAllGameObjectList())
	{
		for (auto* comp : entity->GetComponents())
		{
			comp->Update();
		}
	}*/

	// 默认每帧都更新渲染对象
	// Renderer::OnSceneResolved(scene->GetAllGameObjectList());

	// Input::Update();
	//Audio::Update();
}

bool LitchiEditor::ApplicationEditor::IsRunning() const
{
	return !window->ShouldClose();
}

void LitchiEditor::ApplicationEditor::RenderViews(float p_deltaTime)
{
	// 渲染View 
	auto& sceneView = m_panelsManager.GetPanelAs<SceneView>("Scene View");
	if (sceneView.IsOpened())
	{
		sceneView.UpdateView(p_deltaTime);
		sceneView.Render();
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
	uiManager->Render();
}

void LitchiEditor::ApplicationEditor::SelectActor(GameObject* p_target)
{
	auto name = p_target->GetName();
	auto transform = p_target->GetComponent<Transform>();
	auto position = transform->GetPosition();
	auto rotation = transform->GetRotation();
	auto rotationEuler = rotation.ToEulerAngles();
	DEBUG_LOG_INFO("SelectGO name:{},position:({},{},{}),rotation:({},{},{})", name, position.x, position.y, position.z, rotationEuler.x, rotationEuler.y, rotationEuler.z);

	// todo Inspector 选择

	auto& inspector = m_panelsManager.GetPanelAs<Inspector>("Inspector");
	inspector.FocusActor(p_target);

	m_panelsManager.GetPanelAs<Hierarchy>("Hierarchy").SelectActorByInstance(p_target);
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
	//m_panelsManager.CreatePanel<HardwareInfo>("Hardware Info", false, settings, 0.2f, 50);
	//m_panelsManager.CreatePanel<Profiler>("Profiler", true, settings, 0.25f);
	//m_panelsManager.CreatePanel<Console>("Console", true, settings);
	//m_panelsManager.CreatePanel<GameView>("Game View", true, settings);
	m_panelsManager.CreatePanel<AssetView>("Asset View", false, settings, m_rendererPath4SceneView);
	//m_panelsManager.CreatePanel<Toolbar>("Toolbar", true, settings);
	m_panelsManager.CreatePanel<MaterialEditor>("Material Editor", false, settings);
	//m_panelsManager.CreatePanel<ProjectSettings>("Project Settings", false, settings);
	//m_panelsManager.CreatePanel<AssetProperties>("Asset Properties", false, settings);

	m_canvas.MakeDockspace(true);
	// 
	uiManager->SetCanvas(m_canvas);

}
