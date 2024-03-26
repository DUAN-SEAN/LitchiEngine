
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
		DEBUG_LOG_INFO("ConfigManager::Initialize path: "+ path);
		if(!AssetManager::LoadAsset(path, m_config_res))
		{
			DEBUG_LOG_INFO("ConfigManager::Create Default Config: " + path);

			// Init Default Config
			InitDefaultConfig(projectRootFolder);

			Save();
		}

		//const bool NeedInitAndSave = true;
		//if(NeedInitAndSave)
		//{
		//	InitDefaultConfig(projectRootFolder);

		//	Save();
		//}

		return true;
	}

	const std::string& ConfigManager::GetRootFolder() const
	{
		return m_projectRootFolder;
	}

	const std::string ConfigManager::GetAssetFolder() const
	{
		return m_projectRootFolder+m_config_res.m_asset_folder;
	}

	const std::string ConfigManager::GetScriptFolder() const
	{
		return m_projectRootFolder+m_config_res.m_script_folder;
	}

	const std::string ConfigManager::GetDefaultScenePath() const
	{
		return m_projectRootFolder+m_config_res.m_default_scene_path;
	}

	const std::pair<int32_t, int32_t> ConfigManager::GetResolutionSize()
	{
		return std::pair(m_config_res.m_resolution_width, m_config_res.m_resolution_height);
	}

	const bool ConfigManager::IsFullScreen()
	{
		return m_config_res.m_is_fullScreen;
	}

	bool ConfigManager::Save()
	{
		auto path = m_projectRootFolder + ConfigFileName;
		DEBUG_LOG_INFO("ConfigManager::Save Config Path: {}", path);

		if (!AssetManager::SaveAsset(m_config_res, path))
		{
			DEBUG_LOG_INFO("ConfigManager::Save Config Fail Path: {}" ,path);

			return false;
		}

		return true;
	}

	void ConfigManager::InitDefaultConfig(const std::string& projectRootFolder)
	{
		m_config_res.m_asset_folder = "Assets\\";
		m_config_res.m_script_folder = "Scripts\\";
		m_config_res.m_resolution_width = 1920;
		m_config_res.m_resolution_height = 1080;
		m_config_res.m_is_fullScreen = true;
		// todo init 
	}
}
