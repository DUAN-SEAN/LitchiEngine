
#include "ConfigManager.h"

#include "AssetManager.h"
#include "Runtime/Core/Log/debug.h"

namespace LitchiRuntime
{
	ConfigManager* ConfigManager::Instance_;

	bool ConfigManager::Initialize(ConfigManager* instance,const std::filesystem::path& config_file_path)
	{
		Instance_ = instance;

		auto path = config_file_path.string();
		DEBUG_LOG_INFO("ConfigManager::Initialize path: "+ path);
		if(!AssetManager::LoadAsset(path, instance->config_res_))
		{
			DEBUG_LOG_INFO("ConfigManager::Create Default Config: " + path);
			// ³õÊ¼»¯Ä¬ÈÏÅäÖÃ
			AssetManager::SaveAsset(instance->config_res_, path);
		}

		return true;
	}

	const std::filesystem::path& ConfigManager::GetRootFolder() const
	{
		return config_res_.root_folder_;
	}

	const std::filesystem::path& ConfigManager::GetAssetFolder() const
	{
		return config_res_.asset_folder_;
	}

	const std::filesystem::path& ConfigManager::GetDefaultScenePath() const
	{
		return config_res_.default_scene_path_;
	}
}
