#pragma once
#include <filesystem>
#include "rttr/type"
namespace LitchiRuntime
{
	struct ConfigRes final
	{
		std::string m_asset_folder;
		std::string m_script_folder;
        std::string m_default_scene_path;

        RTTR_ENABLE()
	};

    class ConfigManager
    {
    public:
        ConfigManager() = default;
    	bool Initialize(const std::string& projectRootFolder);

        const std::string& GetRootFolder() const;
        const std::string GetAssetFolder() const;
        const std::string GetScriptFolder() const;
        const std::string GetDefaultScenePath() const;
    private:
        void InitDefaultConfig(const std::string& projectRootFolder);

    private:
    	ConfigRes m_config_res{};
        std::string m_projectRootFolder{};

    };
} // namespace LitchiRuntime
