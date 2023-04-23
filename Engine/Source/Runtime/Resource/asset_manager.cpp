
#include <filesystem>

#include <Runtime/Resource/asset_manager.h>
#include <Runtime/Core/App/application.h>

namespace LitchiRuntime
{
    std::filesystem::path AssetManager::GetFullPath(const std::string& relative_path)
    {
        return std::filesystem::absolute(Application::GetDataPath() + relative_path);
    }
} // namespace Piccolo