
#pragma once

//= INCLUDES ======================
#include <string>
#include "Runtime/Core/Definitions.h"
//=================================

namespace LitchiRuntime
{
    class RHI_Texture;

    class LC_CLASS ImageImporter
    {
    public:
        static void Initialize();
        static void Shutdown();
        static bool Load(const std::string& file_path, const uint32_t slice_index, RHI_Texture* texture);
    	static void Save(const std::string& file_path, const uint32_t width, const uint32_t height, const uint32_t channel_count, const uint32_t bits_per_channel, void* data);
    };
}
