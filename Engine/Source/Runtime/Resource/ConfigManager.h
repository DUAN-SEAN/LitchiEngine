#pragma once
#include <filesystem>
#include "rttr/type"
namespace LitchiRuntime
{
	struct ConfigRes final
	{
		std::string m_root_folder;
		std::string m_asset_folder;
		std::string m_script_folder;
		std::string m_engine_asset_folder;
        std::string m_default_scene_path;

        RTTR_ENABLE()
	};

    class ConfigManager
    {
    public:
        ConfigManager() = default;
    	bool Initialize(const std::filesystem::path& config_file_path);

        const std::filesystem::path& GetRootFolder() const;
        const std::filesystem::path& GetAssetFolder() const;
        const std::filesystem::path& GetEngineAssetFolder() const;
        const std::filesystem::path& GetScriptFolder() const;
        const std::filesystem::path& GetDefaultScenePath() const;
    private:
        void InitDefaultConfig(const std::filesystem::path& config_file_path);

    private:
    	ConfigRes m_config_res;

    };
} // namespace LitchiRuntime
