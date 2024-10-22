
#pragma once

//= INCLUDES ============
#include "RHI_Texture.h"
#include "RHI_Viewport.h"
//=======================

namespace LitchiRuntime
{
    class LC_CLASS RHI_Texture2D : public RHI_Texture
    {
    public:
        // Creates a texture without data (intended for manual loading)
        RHI_Texture2D(const uint32_t flags = RHI_Texture_Srv, const char* name = nullptr)
        {
            m_resource_type = ResourceType::Texture2d;
            m_flags         = flags;

            if (name != nullptr)
            {
                m_object_name = name;
            }
        }

        // Creates a texture from data (intended for sampling)
        RHI_Texture2D(const uint32_t width, const uint32_t height, const RHI_Format format, const uint32_t flags, const std::vector<RHI_Texture_Slice>& data, const char* name = nullptr)
        {
            m_resource_type    = ResourceType::Texture2d;
            m_width            = width;
            m_height           = height;
            m_viewport         = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
            m_format           = format;
            m_data             = data;
            m_mip_count        = GetSlice(0).GetMipCount();
            m_flags            = flags;
            m_channel_count    = rhi_to_format_channel_count(m_format);
            m_bits_per_channel = rhi_format_to_bits_per_channel(m_format);

            if (name != nullptr)
            {
                m_object_name = name;
            }

            RHI_Texture2D::RHI_CreateResource();
            m_is_ready_for_use = true;
        }

        // Creates a texture without any data (intended for usage as a render target)
        RHI_Texture2D(const uint32_t width, const uint32_t height, const uint32_t mip_count, const RHI_Format format, const uint32_t flags, const char* name = nullptr)
        {
            m_resource_type = ResourceType::Texture2d;
            m_width         = width;
            m_height        = height;
            m_viewport      = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
            m_format        = format;
            m_mip_count     = mip_count;
            m_flags         = flags;
            m_channel_count = rhi_to_format_channel_count(format);

            if (name != nullptr)
            {
                m_object_name = name;
            }

            RHI_Texture2D::RHI_CreateResource();
            m_is_ready_for_use = true;
        }

        ~RHI_Texture2D() = default;
    };
}
