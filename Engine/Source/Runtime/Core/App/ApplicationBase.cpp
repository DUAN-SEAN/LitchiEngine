
#include "ApplicationBase.h"

#include <memory>
#include "Runtime/Core/pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

#include "Runtime/Core/Meta/Reflection/type.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/Physics/physics.h"
#include "Runtime/Function/Renderer/Resource/Import/FontImporter.h"
#include "Runtime/Function/Renderer/Resource/Import/ImageImporter.h"
#include "Runtime/Function/Renderer/Resource/Import/ModelImporter.h"

#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"

//#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
	ApplicationBase* ApplicationBase::s_instance;
	void ApplicationBase::Init() {
		s_instance = this;

		m_engineAssetsPath = std::filesystem::canonical("Data\\Engine").string() + "\\";

		// 第二个参数支持后续修改
		DEBUG_LOG_INFO("ConfigManager::Initialize ProjectPath:{}", m_projectPath);

		if(!m_projectPath.empty())
		{
			configManager = std::make_unique<ConfigManager>();
			if (!configManager->Initialize(m_projectPath + "ProjectConfig.Litchi"))
			{
				DEBUG_LOG_ERROR("ConfigManager::Initialize Fail! ProjectPath:{}", m_projectPath);
				return;
			}
		}
		

		// Easy Profiler
		EASY_MAIN_THREAD;
		profiler::startListen();// 启动profiler服务器，等待gui连接。

		Debug::Initialize();

		std::string projectAssetsPath = "";
		if(configManager)
		{
			projectAssetsPath = configManager->GetAssetFolder();
		}

		FileSystem::SetAssetDirectoryPath(projectAssetsPath, m_engineAssetsPath);

		// 初始化场景 如果没有场景则构建默认场景
		sceneManager = std::make_unique<SceneManager>();
		shaderManager = std::make_unique<ShaderManager>();
		materialManager = std::make_unique<MaterialManager>();
		fontManager = std::make_unique<FontManager>();
		textureManager = std::make_unique<TextureManager>();
		modelManager = std::make_unique<ModelManager>();
		prefabManager = std::make_unique<PrefabManager>();

		ServiceLocator::Provide(*sceneManager.get());
		ServiceLocator::Provide(*shaderManager.get());
		ServiceLocator::Provide(*materialManager.get());
		ServiceLocator::Provide(*textureManager.get());
		ServiceLocator::Provide(*modelManager.get());
		ServiceLocator::Provide(*fontManager.get());
		ServiceLocator::Provide(*prefabManager.get());

		/*ModelManager::ProvideAssetPaths(projectAssetsPath);
		ShaderManager::ProvideAssetPaths(projectAssetsPath); */

		DEBUG_LOG_INFO("game start");

		Time::Initialize();

		////初始化图形库，例如glfw
		//InitGraphicsLibraryFramework();

		FontImporter::Initialize();
		ModelImporter::Initialize();
		ImageImporter::Initialize();

		WindowSettings windowSettings;
		windowSettings.title = "Litchi Editor";
		windowSettings.width = 1280;
		windowSettings.height = 720;
		windowSettings.maximized = true;
		// 初始化Window
		window = std::make_unique<Window>(windowSettings);
		{
			auto iconPath = m_engineAssetsPath + "Icons\\Icon.png";
			int iconWidth = 30;
			int iconHeight = 30;
			int iconChannel = 3;
			unsigned char* dataBuffer = stbi_load(iconPath.c_str(), &iconWidth, &iconHeight, &iconChannel, 4);
			window->SetIconFromMemory(reinterpret_cast<uint8_t*>(dataBuffer), iconWidth, iconHeight);
			window->MakeCurrentContext();
		}

		InputManager::Initialize(window.get());

		UpdateScreenSize();

		{
			// ResourceCache::Initialize(m_projectPath);

			Renderer::Initialize();

		}

		TypeManager::Initialize();
		SerializerManager::Initialize();

		//初始化 fmod
		//Audio::Init();

		//初始化物理引擎
		Physics::Init();

		// ScriptEngine::Init(m_projectPath);


	}

	/// 初始化图形库，例如glfw
	void ApplicationBase::InitGraphicsLibraryFramework() {

	}

	void ApplicationBase::InitLuaBinding() {
	}

	void ApplicationBase::LoadConfig() {
	}

	void ApplicationBase::Run() {

	}

	void ApplicationBase::Update() {
		Time::Update();
		UpdateScreenSize();

		InputManager::Tick();

		auto scene = this->sceneManager->GetCurrentScene();
		if (scene)
		{
			scene->Update();
		}

		// Input::Update();
		//Audio::Update();
	}


	void ApplicationBase::Render() {

		// RenderSystem::Instance()->Render();
		Renderer::Tick();
	}

	void ApplicationBase::FixedUpdate() {
		//Physics::FixedUpdate();


		this->sceneManager->Foreach([](GameObject* game_object) {
			if (game_object->GetActive()) {
				game_object->ForeachComponent([](Component* component) {
					component->OnFixedUpdate();
					});
			}
			});
	}

	void ApplicationBase::OneFrame() {
		Update();
		// 如果一帧卡了很久，就多执行几次FixedUpdate
		float cost_time = Time::delta_time();
		while (cost_time >= Time::fixed_update_time()) {
			FixedUpdate();
			cost_time -= Time::fixed_update_time();
		}

		Render();
	}

	void ApplicationBase::UpdateScreenSize() {
	}

	void ApplicationBase::Exit() {
	}
}