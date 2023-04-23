#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include "runtime/core/meta/serializer/serializer.h"

namespace LitchiRuntime
{
    class AssetManager
    {
    public:
        template<typename AssetType>
        static bool LoadAsset(const std::string& asset_url, AssetType& out_asset)
        {
            // read json file to string
            std::filesystem::path asset_path = GetFullPath(asset_url);
            std::ifstream asset_json_file(asset_path);
            if (!asset_json_file)
            {
                DEBUG_LOG_ERROR("open file: {} failed!", asset_path.generic_string());
                return false;
            }

            std::stringstream buffer;
            buffer << asset_json_file.rdbuf();
            std::string asset_json_text(buffer.str());
            
            return SerializerManager::DeserializeFromJson(asset_json_text, out_asset);
        }

        template<typename AssetType>
        static bool SaveAsset(const AssetType& out_asset, const std::string& asset_url)
        {
            std::ofstream asset_json_file(GetFullPath(asset_url));
            if (!asset_json_file)
            {
                DEBUG_LOG_ERROR("open file {} failed!", asset_url);
                return false;
            }

            // write to json object and dump to string
            auto&&        asset_json      = SerializerManager::SerializeToJson(out_asset);
            std::string&& asset_json_text = asset_json.dump();

            // write to file
            asset_json_file << asset_json_text;
            asset_json_file.flush();

            return true;
        }

        static std::filesystem::path GetFullPath(const std::string& relative_path);

    };
} // namespace Piccolo
