
#include "ConfigManager.h"

#include "AssetManager.h"
#include "Runtime/Core/Log/debug.h"

namespace LitchiRuntime
{
	bool ConfigManager::Initialize(const std::filesystem::path& config_file_path)
	{
		auto path = config_file_path.string();
		DEBUG_LOG_INFO("ConfigManager::Initialize path: "+ path);
		if(!AssetManager::LoadAsset(path, m_config_res))
		{
			DEBUG_LOG_INFO("ConfigManager::Create Default Config: " + path);

			// Init Default Config
			InitDefaultConfig(config_file_path);

			AssetManager::SaveAsset(m_config_res, path);
		}

		return true;
	}

	const std::filesystem::path& ConfigManager::GetRootFolder() const
	{
		return m_config_res.m_root_folder;
	}

	const std::filesystem::path& ConfigManager::GetAssetFolder() const
	{
		return m_config_res.m_asset_folder;
	}

	const std::filesystem::path& ConfigManager::GetEngineAssetFolder() const
	{
		return m_config_res.m_engine_asset_folder;
	}

	const std::filesystem::path& ConfigManager::GetScriptFolder() const
	{
		return m_config_res.m_script_folder;
	}

	const std::filesystem::path& ConfigManager::GetDefaultScenePath() const
	{
		return m_config_res.m_default_scene_path;
	}

	void ConfigManager::InitDefaultConfig(const std::filesystem::path& config_file_path)
	{
		// m_config_res.m_engine_asset_folder =
		m_config_res.m_root_folder = config_file_path.string();
		m_config_res.m_asset_folder = m_config_res.m_root_folder + "Assets//";
		m_config_res.m_script_folder = m_config_res.m_script_folder + "Scripts//";
		// todo init 


	}
}
