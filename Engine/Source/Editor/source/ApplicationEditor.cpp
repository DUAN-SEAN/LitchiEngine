
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
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Resources/Loaders/ModelLoader.h"
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"
#include "stb_image.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include <Runtime/Function/Framework/Component/UI/UIText.h>

LitchiEditor::ApplicationEditor* LitchiEditor::ApplicationEditor::instance_;
struct data
{
	union
	{
		float float_value;
		unsigned char byte_value[4];
	};
};
LitchiEditor::ApplicationEditor::ApplicationEditor() :m_canvas(), m_panelsManager(m_canvas), m_editorActions(m_panelsManager)
{
	char* projectPath = nullptr;
	projectPath = _getcwd(nullptr, 1);
	std::string filePath(projectPath);
	editorAssetsPath = filePath + "\\..\\..\\Assets\\";

	// todo 暂时这样写
	projectAssetsPath = editorAssetsPath;

	ModelManager::ProvideAssetPaths(projectAssetsPath);
	TextureManager::ProvideAssetPaths(projectAssetsPath);
	ShaderManager::ProvideAssetPaths(projectAssetsPath);
	MaterialManager::ProvideAssetPaths(projectAssetsPath);
	FontManager::ProvideAssetPaths(projectAssetsPath);
}

LitchiEditor::ApplicationEditor::~ApplicationEditor()
{
}

GameObject* CreateDefaultObject(Scene* scene, std::string name, std::string modelPath, std::string materialPath, float y, float z)
{
	GameObject* go = scene->CreateGameObject(name);
	auto transform = go->AddComponent<Transform>();
	transform->SetLocalPosition(glm::vec3(0.0, y, z));

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
	auto transform = go->AddComponent<Transform>();
	transform->SetLocalPosition(glm::vec3(0.0, y, z));

	auto animator = go->AddComponent<Animator>();

	auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->modelPath = modelPath;
	mesh_filter->PostResourceLoaded();

	auto mesh_renderer = go->AddComponent<SkinnedMeshRenderer>();
	mesh_renderer->materialPath = materialPath;
	mesh_renderer->PostResourceLoaded();
	go->SetLayer(0x01);

	// 初始化animator
	auto* model = mesh_filter->GetModel();
	std::unordered_map<std::string, AnimationClip> animations;
	model->GetAnimations(animations);
	auto firstClipName = animations.begin()->first;
	animator->SetAnimationClipMap(animations);
	animator->Play(firstClipName);

	return go;
}

GameObject* CreateDefaultObject(Scene* scene, std::string name, std::string modelPath, std::string materialPath, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
{
	GameObject* go = scene->CreateGameObject(name);
	auto transform = go->AddComponent<Transform>();
	transform->SetLocalPosition(position);
	transform->SetLocalRotation(rotation);
	transform->SetLocalScale(scale);

	auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->modelPath = modelPath;
	mesh_filter->PostResourceLoaded();

	auto mesh_renderer = go->AddComponent<MeshRenderer>();
	mesh_renderer->materialPath = materialPath;
	mesh_renderer->PostResourceLoaded();
	go->SetLayer(0x01);

	return go;
}

GameObject* CreateLightObject(Scene* scene, std::string name, glm::vec3 pos, glm::quat rotation)
{
	GameObject* go = scene->CreateGameObject(name);

	auto transform = go->AddComponent<Transform>();
	transform->SetLocalPosition(pos);
	transform->SetLocalRotation(rotation);

	auto directionalLight = go->AddComponent<DirectionalLight>();
	directionalLight->SetColor(glm::vec3(0.3, 0.6, 0.7));
	directionalLight->SetIntensity(3.0f);

	return go;
}

GameObject* CreateUIImageObject(Scene* scene, std::string name, glm::vec3 pos, glm::quat rotation, Texture* image)
{
	GameObject* go = scene->CreateGameObject(name);

	auto transform = go->AddComponent<Transform>();
	transform->SetLocalPosition(pos);
	transform->SetLocalRotation(rotation);
	transform->SetLocalScale(glm::vec3(1));

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

GameObject* CreateUITextObject(Scene* scene, std::string name, glm::vec3 pos, glm::quat rotation, Font* font)
{
	GameObject* go = scene->CreateGameObject(name);

	auto transform = go->AddComponent<Transform>();
	transform->SetLocalPosition(pos);
	transform->SetLocalRotation(rotation);
	transform->SetLocalScale(glm::vec3(1));

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
	uiText->SetColor(glm::vec4(1));

	return go;
}

void LitchiEditor::ApplicationEditor::Init()
{
	instance_ = this;

	DeviceSettings deviceSettings;
	deviceSettings.contextMajorVersion = 4;
	deviceSettings.contextMinorVersion = 6;

	WindowSettings windowSettings;
	windowSettings.title = "Litchi Editor";
	windowSettings.width = 1280;
	windowSettings.height = 720;
	windowSettings.maximized = true;

	// 初始化驱动(OpenGL)
	device = std::make_unique<Device>(deviceSettings);
	device->SetVsync(true);

	// 初始化Window
	window = std::make_unique<Window>(*device, windowSettings);
	{
		auto iconPath = editorAssetsPath + "Icon.png";
		int iconWidth = 30;
		int iconHeight = 30;
		int iconChannel = 3;
		unsigned char* dataBuffer = stbi_load(iconPath.c_str(), &iconWidth, &iconHeight, &iconChannel, 4);
		window->SetIconFromMemory(reinterpret_cast<uint8_t*>(dataBuffer), iconWidth, iconHeight);
		window->MakeCurrentContext();
	}

	driver = std::make_unique<Driver>(true);

	ApplicationBase::Init();

	editorResources = std::make_unique<EditorResources>(editorAssetsPath);
	renderer = std::make_unique<Renderer>(*driver);
	renderer->SetCapability(ERenderingCapability::MULTISAMPLE, true);
	shapeDrawer = std::make_unique<ShapeDrawer>(*renderer);

	// 初始化InputManager
	inputManager = std::make_unique<InputManager>(*window);
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

	// 初始化ResourceManager
	modelManager = std::make_unique<ModelManager>();
	materialManager = std::make_unique<MaterialManager>();
	textureManager = std::make_unique<TextureManager>();
	shaderManager = std::make_unique<ShaderManager>();
	fontManager = std::make_unique<FontManager>();


	LitchiRuntime::ServiceLocator::Provide<ModelManager>(*modelManager);
	LitchiRuntime::ServiceLocator::Provide<MaterialManager>(*materialManager);
	LitchiRuntime::ServiceLocator::Provide<TextureManager>(*textureManager);
	LitchiRuntime::ServiceLocator::Provide<ShaderManager>(*shaderManager);
	LitchiRuntime::ServiceLocator::Provide<FontManager>(*fontManager);

	// EditorResource

	editorRenderer = std::make_unique<EditorRenderer>();

	// UBO
	engineUBO = std::make_unique<UniformBuffer>
		(
			/* UBO Data Layout */
			sizeof(glm::mat4) +
			sizeof(glm::mat4) +
			sizeof(glm::mat4) +
			sizeof(glm::mat4) +
			sizeof(float) +
			sizeof(glm::mat4),
			0, 0,
			EAccessSpecifier::STREAM_DRAW
		);

	// Light
	lightSSBO = std::make_unique<ShaderStorageBuffer>(EAccessSpecifier::STREAM_DRAW);

	simulatedLightSSBO = std::make_unique<ShaderStorageBuffer>(EAccessSpecifier::STREAM_DRAW); // Used in Asset View

	std::vector<glm::mat4> simulatedLights;

	FTransform simulatedLightTransform;
	simulatedLightTransform.SetLocalRotation(glm::quat(glm::degrees(glm::vec3{ -45.f, 180.f, 10.f })));

	Light simulatedDirectionalLight(Light::Type::DIRECTIONAL);
	simulatedDirectionalLight.color = { 1.f, 1.f, 1.f };
	simulatedDirectionalLight.intensity = 1.f;

	Light simulatedAmbientLight(Light::Type::AMBIENT_SPHERE);
	simulatedAmbientLight.color = { 0.07f, 0.07f, 0.07f };
	simulatedAmbientLight.intensity = 1.f;
	simulatedAmbientLight.constant = 1000.0f;

	simulatedLights.push_back(simulatedDirectionalLight.GenerateMatrix(simulatedLightTransform));
	simulatedLights.push_back(simulatedAmbientLight.GenerateMatrix(simulatedLightTransform));

	simulatedLightSSBO->SendBlocks<glm::mat4>(simulatedLights.data(), simulatedLights.size() * sizeof(glm::mat4));
	// 

	// Setup UI
	SetupUI();

	auto* font = fontManager->GetResource("Engine\\Fonts\\Ruda-Bold.ttf");
	auto charVec = font->LoadStr("Hello World");

	// 初始化默认场景
	sceneManager = new SceneManager(projectAssetsPath);

	auto scene = sceneManager->CreateScene("Default Scene");
	{
		GameObject* go = CreateDefaultObject(scene, "liubei", "Engine\\Models\\Cube.fbx", "Engine\\Materials\\Default.mat", glm::vec3(0.0f, -1.0f, 0.f), glm::quat(1, 0, 0, 0), glm::vec3(100, 1, 100));
		GameObject* go2 = CreateDefaultObject4Skinned(scene, "diaochan", "Engine\\Models\\Catwalk Walk Forward HighKnees.fbx", "Engine\\Materials\\Default4Skinned.mat", 1, -3);
		GameObject* go3 = CreateDefaultObject(scene, "xiaoqiao", "Engine\\Models\\Sphere.fbx", "Engine\\Materials\\DefaultUnlit.mat", 3.f, 1.5f);
		GameObject* go4 = CreateLightObject(scene, "DirectionalLight", glm::vec3(0, 10, 0), glm::angleAxis(-160.0f, glm::vec3(1, 0, 0)));
		// GameObject* go5 = CreateDefaultObject(scene, "plane", "../Engine/Models/Plane.fbx", "../material/Default.mat",glm::vec3(0.0f),glm::quat(1,0,0,0),glm::vec3(5,0,5));


		// 创建UI物体, UILayer = 2;
		Texture* image5 = this->textureManager->LoadResource("Engine\\Textures\\liuyifei.png");
		GameObject* go5 = CreateUIImageObject(scene, "UIImage01", glm::vec3(0, 0, 0), glm::quat(1, 0, 0, 0), image5);
		GameObject* go6 = CreateUITextObject(scene, "UIText01", glm::vec3(0, 0, 0), glm::quat(1, 0, 0, 0), font);

		auto hierachy = m_panelsManager.GetPanelAs<Hierarchy>("Hierarchy");
		m_panelsManager.GetPanelAs<LitchiEditor::Hierarchy>("Hierarchy").Refresh();
	}
	sceneManager->SetCurrentScene(scene);

	// 创建光照贴图shader
	m_shadowMapShader = shaderManager->LoadResource("Engine\\Shaders\\DepthShader.glsl");
	m_shadowMapShader4Skinned = shaderManager->LoadResource("Engine\\Shaders\\DepthShader4Skinned.glsl");
}

void LitchiEditor::ApplicationEditor::Run()
{
	while (IsRunning())
	{
		// 更新时间
		Time::Update();

		// PreUpdate
		device->PollEvents();

		Update();

		// Update
		// 检测是否删除物体
		// 检测运行模式 Game or no
		// 渲染Views
		RenderViews(Time::delta_time());
		// 渲染UI
		RenderUI();

		// PostUpdate

		window->SwapBuffers();
		inputManager->ClearEvents();
		++m_elapsedFrames;
	}
}

void LitchiEditor::ApplicationEditor::Update()
{
	// todo 目前游戏世界的Tick不完整, 先不处理
	this->sceneManager->Foreach([](GameObject* game_object) {
		if (game_object->GetActive()) {
			game_object->ForeachComponent([](Component* component) {
				component->Update();
				});
		}
		});
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
		sceneView.Update(p_deltaTime);
		sceneView.Render();
	}
	auto& assetView = m_panelsManager.GetPanelAs<AssetView>("Asset View");
	if (assetView.IsOpened())
	{
		simulatedLightSSBO->Bind(0);

		assetView.Update(p_deltaTime);
		assetView.Render();
		simulatedLightSSBO->Unbind();
	}

}

void LitchiEditor::ApplicationEditor::RenderUI()
{
	uiManager->Render();
}

void LitchiEditor::ApplicationEditor::SelectActor(GameObject* p_target)
{
	auto name = p_target->GetName();
	auto transform = p_target->GetComponent<Transform>();
	auto position = transform->GetWorldPosition();
	auto rotation = transform->GetWorldRotation();
	auto rotationEuler = glm::eulerAngles(rotation);
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
	m_panelsManager.CreatePanel<SceneView>("Scene View", true, settings);
	m_panelsManager.CreatePanel<Hierarchy>("Hierarchy", true, settings);
	m_panelsManager.CreatePanel<Inspector>("Inspector", true, settings);
	m_panelsManager.CreatePanel<AssetBrowser>("Asset Browser", true, settings, projectAssetsPath);
	//m_panelsManager.CreatePanel<HardwareInfo>("Hardware Info", false, settings, 0.2f, 50);
	//m_panelsManager.CreatePanel<Profiler>("Profiler", true, settings, 0.25f);
	//m_panelsManager.CreatePanel<Console>("Console", true, settings);
	//m_panelsManager.CreatePanel<Hierarchy>("Hierarchy", true, settings);
	//m_panelsManager.CreatePanel<Inspector>("Inspector", true, settings);
	//m_panelsManager.CreatePanel<SceneView>("Scene View", true, settings);
	//m_panelsManager.CreatePanel<GameView>("Game View", true, settings);
	m_panelsManager.CreatePanel<AssetView>("Asset View", false, settings);
	//m_panelsManager.CreatePanel<Toolbar>("Toolbar", true, settings);
	m_panelsManager.CreatePanel<MaterialEditor>("Material Editor", false, settings);
	//m_panelsManager.CreatePanel<ProjectSettings>("Project Settings", false, settings);
	//m_panelsManager.CreatePanel<AssetProperties>("Asset Properties", false, settings);

	m_canvas.MakeDockspace(true);
	// 
	uiManager->SetCanvas(m_canvas);

}
