
#pragma once

//= INCLUDES ===========
#include "RHI_Texture.h"
//======================

namespace LitchiRuntime
{
    class SP_CLASS RHI_TextureCube : public RHI_Texture
    {
    public:
        RHI_TextureCube()
        {
            m_resource_type = ResourceType::TextureCube;
        }

        // Creates a texture from data (intended for sampling)
        RHI_TextureCube(const uint32_t width, const uint32_t height, const RHI_Format format, const uint32_t flags, const std::vector<RHI_Texture_Slice>& data)
        {
            m_resource_type    = ResourceType::TextureCube;
            m_width            = width;
            m_height           = height;
            m_viewport         = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
            m_format           = format;
            m_data             = data;
            m_array_length     = 6;
            m_mip_count        = GetSlice(0).GetMipCount();
            m_flags            = flags;
            m_channel_count    = rhi_to_format_channel_count(m_format);
            m_bits_per_channel = rhi_format_to_bits_per_channel(m_format);

            RHI_TextureCube::RHI_CreateResource();
            m_is_ready_for_use = true;
        }

        // Creates a texture without data (intended for use as a render target)
        RHI_TextureCube(const uint32_t width, const uint32_t height, const RHI_Format format, const uint32_t flags, std::string name = "")
        {
            m_object_name          = name;
            m_resource_type = ResourceType::TextureCube;
            m_width         = width;
            m_height        = height;
            m_viewport      = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
            m_format        = format;
            m_array_length  = 6;
            m_flags         = flags;

            RHI_TextureCube::RHI_CreateResource();
            m_is_ready_for_use = true;
        }

        ~RHI_TextureCube() = default;
    };
}
