
#pragma once

//= INCLUDES =================
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace Spartan
{
    class SP_CLASS RHI_Viewport : public LitchiRuntime::Object
    {
    public:
        RHI_Viewport(const float x = 0.0f, const float y = 0.0f, const float width = 0.0f, const float height = 0.0f, const float depth_min = 0.0f, const float depth_max = 1.0f)
        {
            this->x         = x;
            this->y         = y;
            this->width     = width;
            this->height    = height;
            this->depth_min = depth_min;
            this->depth_max = depth_max;
        }

        RHI_Viewport(const RHI_Viewport& viewport)
        {
            x         = viewport.x;
            y         = viewport.y;
            width     = viewport.width;
            height    = viewport.height;
            depth_min = viewport.depth_min;
            depth_max = viewport.depth_max;
        }

        ~RHI_Viewport() = default;

        bool operator==(const RHI_Viewport& rhs) const
        {
            return 
                x         == rhs.x         && y         == rhs.y && 
                width     == rhs.width     && height    == rhs.height && 
                depth_min == rhs.depth_min && depth_max == rhs.depth_max;
        }

        bool operator!=(const RHI_Viewport& rhs) const
        {
            return !(*this == rhs);
        }

        bool IsDefined() const
        {
            return
                x         != 0.0f || 
                y         != 0.0f || 
                width     != 0.0f || 
                height    != 0.0f || 
                depth_min != 0.0f || 
                depth_max != 0.0f;
        }

        float GetAspectRatio() const { return width / height; }

        float x         = 0.0f;
        float y         = 0.0f;
        float width     = 0.0f;
        float height    = 0.0f;
        float depth_min = 0.0f;
        float depth_max = 0.0f;

        static const RHI_Viewport Undefined;
    };
}
