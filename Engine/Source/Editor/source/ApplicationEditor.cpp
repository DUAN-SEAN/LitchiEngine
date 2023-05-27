
#include "Editor/include/ApplicationEditor.h"

#include <filesystem>

#include "Editor/include/Panels/Hierarchy.h"
#include "Editor/include/Panels/Inspector.h"
#include "Editor/include/Panels/MenuBar.h"
#include "Editor/include/Panels/SceneView.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Renderer/material.h"
#include "Runtime/Function/Renderer/Resources/Loaders/MaterialLoader.h"
#include "Runtime/Function/Renderer/Resources/Loaders/ModelLoader.h"
#include "Runtime/Resource/MaterialManager.h"
#include "Runtime/Resource/ModelManager.h"
#include "Runtime/Resource/ShaderManager.h"
#include "Runtime/Resource/TextureManager.h"


LitchiEditor::ApplicationEditor* LitchiEditor::ApplicationEditor::instance_;

LitchiEditor::ApplicationEditor::ApplicationEditor():m_canvas(),m_panelsManager(m_canvas)
{
	char* projectPath = nullptr;
	projectPath = _getcwd(nullptr, 1);
	std::string filePath(projectPath);
	editorAssetsPath = filePath + "/../../Assets/Editor/";
	engineAssetsPath = filePath + "/../../Assets/Engine/";

	// todo 暂时这样写
	projectAssetsPath = editorAssetsPath;

	ModelManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	TextureManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	ShaderManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	MaterialManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
}

LitchiEditor::ApplicationEditor::~ApplicationEditor()
{
}

GameObject* CreateDefaultObject(Scene* scene, std::string name,Model* model , float y, float z)
{
	GameObject* go = new GameObject(name, scene);
	auto transform = go->AddComponent<Transform>();
	transform->set_position(glm::vec3(0.0, y, z));

	auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->LoadMesh("model/fishsoup_pot.mesh");



	auto mesh_renderer = go->AddComponent<MeshRenderer>();
	Material* material = new Material();//设置材质
	material->Parse("material/materialTemplete2.mat");
	mesh_renderer->SetMaterial(material);
	go->set_layer(0x01);

	return go;
}


GameObject* CreateDefaultObject(Scene* scene,std::string name,float y, float z)
{
	GameObject* go = new GameObject(name, scene);
	auto transform = go->AddComponent<Transform>();
	transform->set_position(glm::vec3(0.0, y, z));
	auto mesh_filter = go->AddComponent<MeshFilter>();
	mesh_filter->LoadMesh("model/fishsoup_pot.mesh");
	auto mesh_renderer = go->AddComponent<MeshRenderer>();
	Material* material = new Material();//设置材质
	material->Parse("material/materialTemplete2.mat");
	mesh_renderer->SetMaterial(material);
	go->set_layer(0x01);

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
		std::vector<uint64_t> iconRaw = { 0,0,144115188614240000,7500771567664627712,7860776967494637312,0,0,0,0,7212820467466371072,11247766461832697600,14274185407633888512,12905091124788992000,5626708973701824512,514575842263176960,0,0,6564302121125019648,18381468271671515136,18381468271654737920,18237353083595659264,18165295488836311040,6708138037527189504,0,4186681893338480640,7932834557741046016,17876782538917681152,11319824055216379904,15210934132358518784,18381468271520454400,1085667680982603520,0,18093237891929479168,18309410677600032768,11391881649237530624,7932834561381570304,17300321784231761408,15210934132375296000,8293405106311272448,2961143145139082752,16507969723533236736,17516777143216379904,10671305705855129600,7356091234422036224,16580027318695106560,2240567205413984000,18381468271470188544,10959253511276599296,4330520004484136960,10815138323200743424,11607771853338181632,8364614976649238272,17444719546862998784,2669156352,18381468269893064448,6419342512197474304,11103650170688640000,6492244531366860800,14346241902646925312,13841557270159628032,7428148827772098304,3464698581331941120,18381468268953606144,1645680384,18381468271554008832,7140201027266418688,5987558797656659712,17588834734687262208,7284033640602212096,14273902834169157632,18381468269087692288,6852253225049397248,17732667349600245504,16291515470083266560,10022503688432981760,11968059825861367552,9733991836700645376,14850363587428816640,18381468271168132864,16147400282007410688,656430432014827520,18381468270950094848,15715054717226194944,72057596690306560,11823944635485519872,15859169905251653376,17084149004500473856,8581352906816952064,2527949855582584832,18381468271419856896,8581352907253225472,252776704,1376441223417430016,14994761349590357760,10527190521537370112,0,9806614576878321664,18381468271671515136,17156206598538401792,6059619689256392448,10166619973990488064,18381468271403079424,17444719549178451968,420746240,870625192710242304,4906133035823863552,18381468269289150464,18381468271671515136,18381468271671515136,9950729769032620032,14778305994951169792,269422336,0,0,18381468268785833984,8941923452686178304,18381468270950094848,3440842496,1233456333565402880,0,0,0,11823944636091210240,2383877888,16724143605745719296,2316834816,0,0 };
		window->SetIconFromMemory(reinterpret_cast<uint8_t*>(iconRaw.data()), 16, 16);
		window->MakeCurrentContext();
	}

	driver = std::make_unique<Driver>(true);

	ApplicationBase::Init();

	editorResources = std::make_unique<EditorResources>(editorAssetsPath);
	renderer = std::make_unique<Renderer>(*driver);
	renderer->SetCapability(ERenderingCapability::MULTISAMPLE, true);
	shapeDrawer = std::make_unique<ShapeDrawer>(*renderer);

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

	// 初始化InputManager
	inputManager = std::make_unique<InputManager>(*window);
	uiManager = std::make_unique<UIManager>(window->GetGlfwWindow(), EStyle::ALTERNATIVE_DARK);
	{
		/*uiManager->LoadFont("Ruda_Big", editorAssetsPath + "\\Fonts\\Ruda-Bold.ttf", 16);
		uiManager->LoadFont("Ruda_Small", editorAssetsPath + "\\Fonts\\Ruda-Bold.ttf", 12);
		uiManager->LoadFont("Ruda_Medium", editorAssetsPath + "\\Fonts\\Ruda-Bold.ttf", 14);
		uiManager->UseFont("Ruda_Medium");*/
		uiManager->SetEditorLayoutSaveFilename(std::string(getenv("APPDATA")) + "\\OverloadTech\\OvEditor\\layout.ini");
		uiManager->SetEditorLayoutAutosaveFrequency(60.0f);
		uiManager->EnableEditorLayoutSave(true);
		uiManager->EnableDocking(true);
	}

	if (!std::filesystem::exists(std::string(getenv("APPDATA")) + "\\OverloadTech\\OvEditor\\layout.ini"))
		uiManager->ResetLayout("Config\\layout.ini");

	// 初始化ResourceManager
	modelManager = std::make_unique<ModelManager>();
	materialManager = std::make_unique<MaterialManager>();
	textureManager = std::make_unique<TextureManager>();
	shaderManager = std::make_unique<ShaderManager>();

	// EditorResource

	// UBO

	// Light 


	// Setup UI
	SetupUI();

	// 初始化默认场景
	sceneManager = new SceneManager();
	// 初始化默认场景
	auto scene = sceneManager->CreateScene("Default Scene");
	{
		GameObject* go = CreateDefaultObject(scene, "liubei", 0, -10);
		auto hierachy = m_panelsManager.GetPanelAs<Hierarchy>("Hierarchy");

		GameObject* go2 = CreateDefaultObject(scene, "diaochan", 10, -30);
		GameObject* go3 = CreateDefaultObject(scene, "xiaoqiao", -10, 0);
		hierachy.AddActorByInstance(go);
		hierachy.AddActorByInstance(go2);
		hierachy.AddActorByInstance(go3);
	}

	// 测试代码
	{
		std::string fbxPath = data_path_ + "model/fbx_extra.fbx";
		std::string fbxPath2 = "../model/fbx_extra.fbx";

		auto sceneJson = SerializerManager::SerializeToJson(*scene);

		auto scene2 = sceneManager->CreateScene("Default Scene");
		SerializerManager::DeserializeFromJson(sceneJson, *scene2);
		DEBUG_LOG_INFO(sceneJson);

		auto model2 = modelManager->LoadResource(fbxPath2);
		auto model = Loaders::ModelLoader::Create(fbxPath);

		auto material = materialManager->LoadResource("../material/Default.mat");

	}
}

void LitchiEditor::ApplicationEditor::Run()
{
	while (IsRunning())
	{
		// 更新时间
		Time::Update();

		// PreUpdate
		device->PollEvents();

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
}

void LitchiEditor::ApplicationEditor::RenderUI()
{
	uiManager->Render();
}

void LitchiEditor::ApplicationEditor::SelectActor(GameObject* p_target)
{
	auto name = p_target->name();
	auto transform = p_target->GetComponent<Transform>();
	auto position = transform->position();
	auto rotation = transform->rotation();
	auto rotationEuler = glm::eulerAngles(rotation);
	DEBUG_LOG_INFO("SelectGO name:{},position:({},{},{}),rotation:({},{},{})", name, position.x, position.y, position.z, rotationEuler.x, rotationEuler.y, rotationEuler.z);

	// todo Inspector 选择

	auto& inspector = m_panelsManager.GetPanelAs<Inspector>("Inspector");
	inspector.FocusActor(p_target);
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

	//m_panelsManager.CreatePanel<AssetBrowser>("Asset Browser", true, settings, engineAssetsPath, projectAssetsPath, projectScriptsPath);
	//m_panelsManager.CreatePanel<HardwareInfo>("Hardware Info", false, settings, 0.2f, 50);
	//m_panelsManager.CreatePanel<Profiler>("Profiler", true, settings, 0.25f);
	//m_panelsManager.CreatePanel<Console>("Console", true, settings);
	//m_panelsManager.CreatePanel<Hierarchy>("Hierarchy", true, settings);
	//m_panelsManager.CreatePanel<Inspector>("Inspector", true, settings);
	//m_panelsManager.CreatePanel<SceneView>("Scene View", true, settings);
	//m_panelsManager.CreatePanel<GameView>("Game View", true, settings);
	//m_panelsManager.CreatePanel<AssetView>("Asset View", false, settings);
	//m_panelsManager.CreatePanel<Toolbar>("Toolbar", true, settings);
	//m_panelsManager.CreatePanel<MaterialEditor>("Material Editor", false, settings);
	//m_panelsManager.CreatePanel<ProjectSettings>("Project Settings", false, settings);
	//m_panelsManager.CreatePanel<AssetProperties>("Asset Properties", false, settings);

	m_canvas.MakeDockspace(true);
	// 
	uiManager->SetCanvas(m_canvas);

}
