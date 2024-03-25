
#pragma once

//= INCLUDES ============
#include "RHI_Texture.h"
#include "RHI_Viewport.h"
//=======================

namespace LitchiRuntime
{
    class LC_CLASS RHI_Texture2DArray : public RHI_Texture
    {
    public:
        RHI_Texture2DArray(const uint32_t flags = RHI_Texture_Srv, const char* name = nullptr)
        {
            m_resource_type = ResourceType::Texture2dArray;
            m_flags         = flags;

            if (name != nullptr)
            {
                m_object_name = name;
            }
        }

        // Creates a texture without any data (intended for usage as a render target)
        RHI_Texture2DArray(const uint32_t width, const uint32_t height, const RHI_Format format, const uint32_t array_length, const uint32_t flags, std::string name = "")
        {
            m_object_name          = name;
            m_resource_type = ResourceType::Texture2dArray;
            m_width         = width;
            m_height        = height;
            m_viewport      = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
            m_format        = format;
            m_array_length  = array_length;
            m_flags         = flags;

            RHI_CreateResource();
            m_is_ready_for_use = true;
        }

        ~RHI_Texture2DArray() = default;
    };
}
