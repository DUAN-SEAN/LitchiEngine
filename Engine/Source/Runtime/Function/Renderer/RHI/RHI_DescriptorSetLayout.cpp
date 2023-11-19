
//= INCLUDES =======================
#include "Runtime/Core/pch.h"
#include "RHI_DescriptorSetLayout.h"
#include "RHI_ConstantBuffer.h"
#include "RHI_StructuredBuffer.h"
#include "RHI_Texture.h"
#include "RHI_DescriptorSet.h"
#include "RHI_Device.h"
//==================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_DescriptorSetLayout::RHI_DescriptorSetLayout(const vector<RHI_Descriptor>& descriptors, const string& name)
    {
        m_descriptors = descriptors;
        // m_object_name = name;

        CreateRhiResource(m_descriptors);

        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            m_hash = rhi_hash_combine(m_hash, descriptor.ComputeHash());
        }
    }

    void RHI_DescriptorSetLayout::SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constant_buffer)
    {
        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            if (descriptor.slot == slot + rhi_shader_shift_register_b)
            {
                // determine if the descriptor set needs to bind (vkCmdBindDescriptorSets)
                m_needs_to_bind = descriptor.data           != constant_buffer              ? true : m_needs_to_bind;
                m_needs_to_bind = descriptor.dynamic_offset != constant_buffer->GetOffset() ? true : m_needs_to_bind;

                // update
                descriptor.data           = static_cast<void*>(constant_buffer); // needed for vkUpdateDescriptorSets()
                descriptor.range          = constant_buffer->GetStride();        // needed for vkUpdateDescriptorSets()
                descriptor.dynamic_offset = constant_buffer->GetOffset();        // needed for vkCmdBindDescriptorSets

                SP_ASSERT_MSG(constant_buffer->GetStructSize() == descriptor.struct_size, "Size mismatch between CPU and GPU side constant buffer");
                SP_ASSERT_MSG(descriptor.dynamic_offset % descriptor.range == 0,          "Incorrect dynamic offset");

                return;
            }
        }
    }

    void RHI_DescriptorSetLayout::SetStructuredBuffer(const uint32_t slot, RHI_StructuredBuffer* structured_buffer)
    {
        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            if (descriptor.slot == slot + rhi_shader_shift_register_u)
            {
                // determine if the descriptor set needs to bind (vkCmdBindDescriptorSets)
                m_needs_to_bind = descriptor.data           != structured_buffer              ? true : m_needs_to_bind;
                m_needs_to_bind = descriptor.dynamic_offset != structured_buffer->GetOffset() ? true : m_needs_to_bind;
                m_needs_to_bind = descriptor.range          != structured_buffer->GetStride() ? true : m_needs_to_bind;

                // update
                descriptor.data           = static_cast<void*>(structured_buffer);
                descriptor.dynamic_offset = structured_buffer->GetOffset();
                descriptor.range          = structured_buffer->GetStride();

                return;
            }
        }
    }

    void RHI_DescriptorSetLayout::SetSampler(const uint32_t slot, RHI_Sampler* sampler)
    {
        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            if (descriptor.slot == slot + rhi_shader_shift_register_s)
            {
                // determine if the descriptor set needs to bind (vkCmdBindDescriptorSets)
                m_needs_to_bind = descriptor.data != sampler ? true : m_needs_to_bind;

                // update
                descriptor.data = static_cast<void*>(sampler);

                return;
            }
        }
    }

    void RHI_DescriptorSetLayout::SetTexture(const uint32_t slot, RHI_Texture* texture, const uint32_t mip_index, const uint32_t mip_range)
    {
        bool mip_specified = mip_index != rhi_all_mips;
        RHI_Image_Layout layout = texture->GetLayout(mip_specified ? mip_index : 0);

         // Validate layout
        SP_ASSERT(layout == RHI_Image_Layout::General || layout == RHI_Image_Layout::Shader_Read_Only_Optimal || layout == RHI_Image_Layout::Depth_Stencil_Read_Only_Optimal);

        // Validate type
        SP_ASSERT(texture->IsSrv());

        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            bool is_storage = layout == RHI_Image_Layout::General;
            uint32_t shift  = is_storage ? rhi_shader_shift_register_u : rhi_shader_shift_register_t;

            if (descriptor.slot == (slot + shift))
            {
                // determine if the descriptor set needs to bind (vkCmdBindDescriptorSets)
                m_needs_to_bind = descriptor.data      != texture   ? true : m_needs_to_bind;
                m_needs_to_bind = descriptor.mip       != mip_index ? true : m_needs_to_bind;
                m_needs_to_bind = descriptor.mip_range != mip_range ? true : m_needs_to_bind;

                // update
                descriptor.data      = static_cast<void*>(texture);
                descriptor.layout    = layout;
                descriptor.mip       = mip_index;
                descriptor.mip_range = mip_range;

                return;
            }
        }
    }

    void RHI_DescriptorSetLayout::SetMaterialGlobalBuffer(void* buffer, const uint32_t bufferSize)
    {
        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            if (descriptor.slot == rhi_shader_shift_register_global)
            {
                // determine if the descriptor set needs to bind (vkCmdBindDescriptorSets)
                m_needs_to_bind = true;

                // update
                descriptor.data = buffer;
                descriptor.range = bufferSize;
                descriptor.dynamic_offset = 0;

                return;
            }
        }
    }

    void RHI_DescriptorSetLayout::ClearDescriptorData()
    {
        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            descriptor.data           = nullptr;
            descriptor.mip            = 0;
            descriptor.mip_range      = 0;
            descriptor.dynamic_offset = 0;
        }
    }

    RHI_DescriptorSet* RHI_DescriptorSetLayout::GetDescriptorSet()
    {
        RHI_DescriptorSet* descriptor_set = nullptr;

        // integrate descriptor data into the hash (anything that can change)
        // todo: hash not full
        uint64_t hash = m_hash;
        for (const RHI_Descriptor& descriptor : m_descriptors)
        {
            hash = rhi_hash_combine(hash, reinterpret_cast<uint64_t>(descriptor.data));
            hash = rhi_hash_combine(hash, static_cast<uint64_t>(descriptor.mip));
            hash = rhi_hash_combine(hash, static_cast<uint64_t>(descriptor.mip_range));
        }

        // if we don't have a descriptor set to match that state, create one
        unordered_map<uint64_t, RHI_DescriptorSet>& descriptor_sets = RHI_Device::GetDescriptorSets();
        const auto it = descriptor_sets.find(hash);
        if (it == descriptor_sets.end())
        {

            // create descriptor set
            descriptor_sets[hash] = RHI_DescriptorSet(m_descriptors, this, m_object_name.c_str());

            // out
            descriptor_set = &descriptor_sets[hash];
        }
        else if(m_needs_to_bind) // retrieve the existing one
        {
            descriptor_set  = &it->second;
            m_needs_to_bind = false;
        }

        return descriptor_set;
    }

    void RHI_DescriptorSetLayout::GetDynamicOffsets(vector<uint32_t>* offsets)
    {
        // offsets should be ordered by the binding slots in the descriptor
        // set layouts, so m_descriptors should already be sorted by slot
        for (RHI_Descriptor& descriptor : m_descriptors)
        {
            if (descriptor.type == RHI_Descriptor_Type::StructuredBuffer || descriptor.type == RHI_Descriptor_Type::ConstantBuffer)
            {
                (*offsets).emplace_back(descriptor.dynamic_offset);
            }
        }
    }
}
