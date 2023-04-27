#pragma once
#include <filesystem>
#include "rttr/type"
namespace LitchiRuntime
{
	struct ConfigRes
	{
		std::string root_folder_;
		std::string asset_folder_;
        std::string default_scene_path_;

        RTTR_ENABLE()
	};

    class ConfigManager
    {
    public:

        static bool Initialize(ConfigManager* instance,const std::filesystem::path& config_file_path);

        const std::filesystem::path& GetRootFolder() const;
        const std::filesystem::path& GetAssetFolder() const;
        const std::filesystem::path& GetDefaultScenePath() const;

        static ConfigManager* Instance_;

    private:
    	ConfigRes config_res_;

    };
} // namespace LitchiRuntime
