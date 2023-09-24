
#pragma once

#include <cstdint>

namespace Spartan
{
    struct DisplayMode
    {
        DisplayMode() = default;
        DisplayMode(const uint32_t width, const uint32_t height, const uint32_t hz, const uint8_t display_index)
        {
            this->width         = width;
            this->height        = height;
            this->hz            = hz;
            this->display_index = display_index;
        }

        bool operator ==(const DisplayMode& rhs) const
        {
            return
                width         == rhs.width  &&
                height        == rhs.height &&
                hz            == rhs.hz     &&
                display_index == rhs.display_index;
        }

        uint32_t width        = 0;
        uint32_t height       = 0;
        uint32_t hz           = 0;
        uint8_t display_index = 0;

    };
}
