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
		int32_t m_resolution_width;
		int32_t m_resolution_height;
		bool m_is_fullScreen;

		RTTR_ENABLE()
	};

	class ConfigManager
	{
	public:
		ConfigManager() = default;
		bool Initialize(const std::string& projectRootFolder);
		void Tick(float delta);

		const std::string& GetRootFolder() const;
		const std::string GetAssetFolderFullPath() const;
		const std::string GetScriptFolderFullPath() const;
		void SetDefaultScenePath(const std::string& defaultScenePath) { m_configRes.m_default_scene_path = defaultScenePath; m_isDirty = true; }
		const std::string GetDefaultScenePath() const;
		const std::pair<int32_t, int32_t> GetResolutionSize();
		const bool IsFullScreen();

		bool Save();
	private:
		void InitDefaultConfig(const std::string& projectRootFolder);

	private:
		ConfigRes m_configRes{};
		std::string m_projectRootFolder{};
		bool m_isDirty{ false };

	};
} // namespace LitchiRuntime
