
#pragma once

//= INCLUDES ===========
#include <vector>

#include "DisplayMode.h"
//======================

namespace Spartan
{
    class SP_CLASS Display
    {
    public:
        // Display modes
        static void DetectDisplayModes();
        static void RegisterDisplayMode(const uint32_t width, const uint32_t height, const uint32_t hz, const uint8_t display_index);
        static const std::vector<DisplayMode>& GetDisplayModes();

        // Properties of the display which contains the engine window at this moment
        static uint32_t GetWidth();
        static uint32_t GetHeight();
        static uint32_t GetRefreshRate();
        static uint32_t GetIndex();
        static bool GetHdr();
        static float GetLuminanceMax();
    };
}
