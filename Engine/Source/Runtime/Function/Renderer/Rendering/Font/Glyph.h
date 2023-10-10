
#pragma once

namespace LitchiRuntime
{
    struct Glyph
    {
        int32_t offset_x            = 0;
        int32_t offset_y            = 0;
        uint32_t width              = 0;
        uint32_t height             = 0;
        uint32_t horizontal_advance = 0;
        float uv_x_left             = 0.0f;
        float uv_x_right            = 0.0f;
        float uv_y_top              = 0.0f;
        float uv_y_bottom           = 0.0f;
    };
}
