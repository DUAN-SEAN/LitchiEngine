
#include "Runtime/Core/pch.h"
#include "ConfigManager.h"

#include "AssetManager.h"
#include "Runtime/Core/Log/debug.h"

namespace LitchiRuntime
{
	const std::string ConfigFileName = "ProjectConfig.litchiProject";

	bool ConfigManager::Initialize(const std::string& projectRootFolder)
	{
		m_projectRootFolder = projectRootFolder;
		auto path = m_projectRootFolder + ConfigFileName;
		DEBUG_LOG_INFO("ConfigManager::Initialize path: " + path);
		if (!AssetManager::LoadAsset(path, m_configRes))
		{
			DEBUG_LOG_INFO("ConfigManager::Create Default Config: " + path);

			// Init Default Config
			InitDefaultConfig(projectRootFolder);

			Save();
		}

		m_dataIniFile = IniFile(m_configRes.m_data);

		//const bool NeedInitAndSave = true;
		//if(NeedInitAndSave)
		//{
		//	InitDefaultConfig(projectRootFolder);

		//	Save();
		//}

		return true;
	}

	void ConfigManager::Tick(float delta)
	{
		if (m_isDirty)
		{
			m_configRes.m_data = m_dataIniFile.GetData();
			Save();
			m_isDirty = false;
		}
	}

	const std::string& ConfigManager::GetRootFolder() const
	{
		return m_projectRootFolder;
	}

	const std::string ConfigManager::GetAssetFolderFullPath() const
	{
		return m_projectRootFolder + m_configRes.m_asset_folder;
	}

	const std::string ConfigManager::GetScriptFolderFullPath() const
	{
		return m_projectRootFolder + m_configRes.m_script_folder;
	}

	const std::string ConfigManager::GetDefaultScenePath() const
	{
		return m_configRes.m_default_scene_path;
	}

	const std::pair<int32_t, int32_t> ConfigManager::GetResolutionSize()
	{
		return std::pair(m_configRes.m_resolution_width, m_configRes.m_resolution_height);
	}

	const bool ConfigManager::IsFullScreen()
	{
		return m_configRes.m_is_fullScreen;
	}

	bool ConfigManager::Save()
	{
		auto path = m_projectRootFolder + ConfigFileName;
		DEBUG_LOG_INFO("ConfigManager::Save Config Path: {}", path);

		if (!AssetManager::SaveAsset(m_configRes, path))
		{
			DEBUG_LOG_INFO("ConfigManager::Save Config Fail Path: {}", path);

			return false;
		}

		return true;
	}

	void ConfigManager::ResetProjectSetting()
	{
		m_dataIniFile.RemoveAll();
		m_dataIniFile.Add<float>("gravity", -9.81f);
		m_dataIniFile.Add<int>("x_resolution", 1280);
		m_dataIniFile.Add<int>("y_resolution", 720);
		m_dataIniFile.Add<bool>("fullscreen", false);
		m_dataIniFile.Add<std::string>("executable_name", "Game");
		m_dataIniFile.Add<std::string>("start_scene", "Scene.ovscene");
		m_dataIniFile.Add<bool>("vsync", true);
		m_dataIniFile.Add<bool>("multisampling", true);
		m_dataIniFile.Add<int>("samples", 4);
		m_dataIniFile.Add<int>("opengl_major", 4);
		m_dataIniFile.Add<int>("opengl_minor", 3);
		m_dataIniFile.Add<bool>("dev_build", true);
		Save();
	}

	void ConfigManager::InitDefaultConfig(const std::string& projectRootFolder)
	{
		m_configRes.m_asset_folder = "Assets/";
		m_configRes.m_script_folder = "Scripts/";
		m_configRes.m_resolution_width = 1920;
		m_configRes.m_resolution_height = 1080;
		m_configRes.m_is_fullScreen = true;
		// todo init 
	}
}
