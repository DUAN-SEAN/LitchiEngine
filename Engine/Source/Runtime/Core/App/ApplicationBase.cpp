﻿
#include "Runtime/Core/pch.h"
#include "ApplicationBase.h"

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
	bool ApplicationBase::Initialize() {
		s_instance = this;

		m_engineRootPath = PathParser::MakeNonWindowsStyle(std::filesystem::current_path().string() + "/");
		m_engineAssetsPath = PathParser::MakeNonWindowsStyle(std::filesystem::canonical("Data/Engine").string() + "/");

		DEBUG_LOG_INFO("ConfigManager::Initialize ProjectPath:{}", m_projectPath);

		if (GetApplicationType() == LitchiApplicationType::Game)
		{
			configManager = std::make_unique<ConfigManager>();
			if (!configManager->Initialize(m_projectPath))
			{
				configManager = nullptr;
				DEBUG_LOG_ERROR("ConfigManager::Initialize Fail! ProjectPath:{}", m_projectPath);
				return false;
			}

		}
		else
		{
			if (!m_projectPath.empty())
			{
				configManager = std::make_unique<ConfigManager>();
				if (!configManager->Initialize(m_projectPath))
				{
					DEBUG_LOG_ERROR("ConfigManager::Initialize Fail! ProjectPath:{}", m_projectPath);
					return false;
				}
			}

		}

		// Easy Profiler
		EASY_MAIN_THREAD;
		profiler::startListen();// 启动profiler服务器，等待gui连接。

		Debug::Initialize();

		std::string projectAssetsPath = "";
		if (configManager)
		{
			projectAssetsPath = configManager->GetAssetFolderFullPath();
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

		Time::Initialize();

		FontImporter::Initialize();
		ModelImporter::Initialize();
		ImageImporter::Initialize();

		WindowSettings windowSettings = CreateWindowSettings();

		// 初始化Window
		window = std::make_unique<Window>(windowSettings);
		{
			auto iconPath = m_engineAssetsPath + "Icons/Icon.png";
			int iconWidth = 30;
			int iconHeight = 30;
			int iconChannel = 3;
			unsigned char* dataBuffer = stbi_load(iconPath.c_str(), &iconWidth, &iconHeight, &iconChannel, 4);
			window->SetIconFromMemory(reinterpret_cast<uint8_t*>(dataBuffer), iconWidth, iconHeight);
			window->MakeCurrentContext();
		}

		InputManager::Initialize(window.get());

		{
			// ResourceCache::Initialize(m_projectPath);

			Renderer::Initialize();
			if(configManager)
			{
				OnApplyProjectSettings();
			}
		}

		//初始化 fmod
		//Audio::Init();

		//初始化物理引擎
		Physics::Init();

		// ScriptEngine::Init(m_projectPath);

		// Register Event
		ServiceLocator::Get<SceneManager>().SceneLoadEvent += std::bind(&ApplicationBase::OnSceneLoaded, this);

		DEBUG_LOG_INFO("ApplicationBase Initialization End");

		return true;

	}

	void ApplicationBase::Run()
	{

	}

	void ApplicationBase::Update()
	{
		Time::Update();
		InputManager::Tick();

		configManager->Tick(Time::GetDeltaTime());
	}

	void ApplicationBase::Exit()
	{
	}
	void ApplicationBase::OnSceneLoaded()
	{
		// donothing
	}
	void ApplicationBase::OnApplyProjectSettings()
	{
		if(configManager)
		{
			auto& inFile = configManager->GetDataIniFile();
			Renderer::SetOption(Renderer_Option::Hdr, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Hdr)));
			Renderer::SetOption(Renderer_Option::Aabb, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Aabb)));
			Renderer::SetOption(Renderer_Option::Lights, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Lights)));
			Renderer::SetOption(Renderer_Option::Anisotropy, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::Anisotropy)));
			Renderer::SetOption(Renderer_Option::Vsync, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Vsync)));
			Renderer::SetOption(Renderer_Option::Grid, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Grid)));
			Renderer::SetOption(Renderer_Option::Physics, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Physics)));
			Renderer::SetOption(Renderer_Option::SelectionOutline, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::SelectionOutline)));
			Renderer::SetOption(Renderer_Option::Wireframe, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Wireframe)));
			Renderer::SetOption(Renderer_Option::TransformHandle, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::TransformHandle)));
			Renderer::SetOption(Renderer_Option::ResolutionScale, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::ResolutionScale)));
			Renderer::SetOption(Renderer_Option::Exposure, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::Exposure)));
			Renderer::SetOption(Renderer_Option::ShadowResolution, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::ShadowResolution)));
			Renderer::SetOption(Renderer_Option::Antialiasing, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::Antialiasing)));
		}
	}

	void ApplicationBase::OnResetProjectSettings()
	{
		if (configManager)
		{
			auto& inFile = configManager->GetDataIniFile();
			Renderer::SetOption(Renderer_Option::Hdr, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Hdr)));
			Renderer::SetOption(Renderer_Option::Aabb, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Aabb)));
			Renderer::SetOption(Renderer_Option::Lights, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Lights)));
			Renderer::SetOption(Renderer_Option::Anisotropy, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::Anisotropy)));
			Renderer::SetOption(Renderer_Option::Vsync, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Vsync)));
			Renderer::SetOption(Renderer_Option::Grid, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Grid)));
			Renderer::SetOption(Renderer_Option::Physics, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Physics)));
			Renderer::SetOption(Renderer_Option::SelectionOutline, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::SelectionOutline)));
			Renderer::SetOption(Renderer_Option::Wireframe, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::Wireframe)));
			Renderer::SetOption(Renderer_Option::TransformHandle, inFile.Get<bool>(ENUM_TO_STRING(Renderer_Option::TransformHandle)));
			Renderer::SetOption(Renderer_Option::ResolutionScale, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::ResolutionScale)));
			Renderer::SetOption(Renderer_Option::Exposure, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::Exposure)));
			Renderer::SetOption(Renderer_Option::ShadowResolution, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::ShadowResolution)));
			Renderer::SetOption(Renderer_Option::Antialiasing, inFile.Get<float>(ENUM_TO_STRING(Renderer_Option::Antialiasing)));
		}
	}
}