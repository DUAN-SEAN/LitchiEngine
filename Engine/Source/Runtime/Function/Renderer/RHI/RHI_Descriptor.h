 
#pragma once

//= INCLUDES ===============
#include "RHI_Definitions.h"
#include "Runtime/Function/Renderer/Rendering/ShaderUniform.h"
//==========================

namespace LitchiRuntime
{
    class RHI_Descriptor
    {
    public:
        RHI_Descriptor() = default;

        RHI_Descriptor(const RHI_Descriptor& descriptor)
        {
            type         = descriptor.type;
            layout       = descriptor.layout;
            slot         = descriptor.slot;
            stage        = descriptor.stage;
            name         = descriptor.name;
            mip          = descriptor.mip;
            array_length = descriptor.array_length;
            struct_size  = descriptor.struct_size;
            as_array = descriptor.as_array;
            isMaterial = descriptor.isMaterial;
            uniformList = descriptor.uniformList;
        }

        RHI_Descriptor(
            const std::string& name,
            const RHI_Descriptor_Type type,
            const RHI_Image_Layout layout,
            const uint32_t slot,
            const uint32_t array_length,
            const uint32_t stage,
            const uint32_t struct_size,
            const bool as_array,
            const uint32_t isMaterial,
            std::vector<ShaderUniform>* uniformList
        )
        {
            this->type         = type;
            this->layout       = layout;
            this->slot         = slot;
            this->stage        = stage;
            this->name         = name;
            this->array_length = array_length;
            this->struct_size  = struct_size;
            this->as_array = as_array;
            this->isMaterial  = isMaterial;
            this->uniformList = uniformList;
        }

        uint64_t ComputeHash()
        {
            if (m_hash == 0)
            {
                m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(slot));
                m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(stage));
            }

            return m_hash;
        }

        bool IsStorage() const { return type == RHI_Descriptor_Type::TextureStorage; }

        // Properties that affect the descriptor hash (static - reflected)
        uint32_t slot  = 0;
        uint32_t stage = 0;

        // Properties that affect the descriptor set hash (dynamic - renderer)
        uint32_t mip       = 0;
        uint32_t mip_range = 0;
        void* data         = nullptr;

        // Properties that don't affect any hash
        RHI_Descriptor_Type type = RHI_Descriptor_Type::Max;
        RHI_Image_Layout layout  = RHI_Image_Layout::Max;
        uint64_t range           = 0;
        uint32_t array_length    = 0;
        uint32_t dynamic_offset  = 0;
        uint32_t struct_size     = 0;
        bool as_array = false;

        // Debugging
        std::string name;

        // global var 
        bool isMaterial;
        // only in material shader
        std::vector<ShaderUniform>* uniformList = nullptr;

    private:
        uint64_t m_hash = 0;
    };
}
