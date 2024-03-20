
#include "ConfigManager.h"

#include "AssetManager.h"
#include "Runtime/Core/Log/debug.h"

namespace LitchiRuntime
{
	bool ConfigManager::Initialize(const std::string& projectRootFolder)
	{
		auto path = projectRootFolder +"ProjectConfig.litchiProject";
		DEBUG_LOG_INFO("ConfigManager::Initialize path: "+ path);
		if(!AssetManager::LoadAsset(path, m_config_res))
		{
			DEBUG_LOG_INFO("ConfigManager::Create Default Config: " + path);

			// Init Default Config
			InitDefaultConfig(projectRootFolder);

			AssetManager::SaveAsset(m_config_res, path);
		}

		return true;
	}

	const std::string& ConfigManager::GetRootFolder() const
	{
		return m_config_res.m_root_folder;
	}

	const std::string& ConfigManager::GetAssetFolder() const
	{
		return m_config_res.m_asset_folder;
	}

	const std::string& ConfigManager::GetScriptFolder() const
	{
		return m_config_res.m_script_folder;
	}

	const std::string& ConfigManager::GetDefaultScenePath() const
	{
		return m_config_res.m_default_scene_path;
	}

	void ConfigManager::InitDefaultConfig(const std::string& projectRootFolder)
	{
		// m_config_res.m_engine_asset_folder =
		m_config_res.m_root_folder = projectRootFolder;
		m_config_res.m_asset_folder = m_config_res.m_root_folder + "Assets\\";
		m_config_res.m_script_folder = m_config_res.m_root_folder + "Scripts\\";
		// todo init 


	}
}
