
//= INCLUDES ==========================
#include "Runtime/Core/pch.h"
#include "../RHI_Implementation.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_Device.h"
//=====================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_DescriptorSetLayout::~RHI_DescriptorSetLayout()
    {
        if (m_rhi_resource)
        {
            RHI_Device::DeletionQueueAdd(RHI_Resource_Type::DescriptorSetLayout, m_rhi_resource);
            m_rhi_resource = nullptr;
        }
    }

    void RHI_DescriptorSetLayout::CreateRhiResource(vector<RHI_Descriptor> descriptors)
    {
        LC_ASSERT(m_rhi_resource == nullptr);

        // remove push constants since they are not part of the descriptor set layout
        descriptors.erase
        (
            remove_if(descriptors.begin(), descriptors.end(), [](RHI_Descriptor& descriptor)
                {
                    return descriptor.type == RHI_Descriptor_Type::PushConstantBuffer ||          // push constants are not part of the descriptor set layout
                        (descriptor.as_array && descriptor.array_length == rhi_max_array_size); // binldess arrays have their own layout
                }),
            descriptors.end()
        );

        // ensure unique binding numbers
        {
            unordered_set<uint32_t> unique_bindings;
            vector<VkDescriptorSetLayoutBinding> duplicate_bindings;

            for (const auto& descriptor : descriptors)
            {
                if (!unique_bindings.insert(descriptor.slot).second)
                {
                    // if insertion failed, the binding number is not unique, store it for inspection
                    duplicate_bindings.push_back
                    ({
                        descriptor.slot,                                   // binding
                        static_cast<VkDescriptorType>(descriptor.type),    // descriptorType
                        descriptor.as_array ? descriptor.array_length : 1, // descriptorCount
                        descriptor.stage,                                  // stageFlags
                        nullptr                                            // pImmutableSamplers
                        });
                }
            }

            LC_ASSERT(duplicate_bindings.empty());
        }

        // layout bindings
        constexpr uint8_t descriptors_max = 255;
        static array<VkDescriptorSetLayoutBinding, descriptors_max> layout_bindings;
        static array<VkDescriptorBindingFlags, descriptors_max> layout_binding_flags;

        for (uint32_t i = 0; i < static_cast<uint32_t>(descriptors.size()); i++)
        {
            const RHI_Descriptor& descriptor = descriptors[i];

            // stage flags
            VkShaderStageFlags stage_flags = 0;
            stage_flags |= (descriptor.stage & RHI_Shader_Vertex) ? VK_SHADER_STAGE_VERTEX_BIT : 0;
            stage_flags |= (descriptor.stage & RHI_Shader_Hull) ? VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT : 0;
            stage_flags |= (descriptor.stage & RHI_Shader_Domain) ? VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT : 0;
            stage_flags |= (descriptor.stage & RHI_Shader_Pixel) ? VK_SHADER_STAGE_FRAGMENT_BIT : 0;
            stage_flags |= (descriptor.stage & RHI_Shader_Compute) ? VK_SHADER_STAGE_COMPUTE_BIT : 0;

            layout_bindings[i].descriptorType     = static_cast<VkDescriptorType>(RHI_Device::GetDescriptorType(descriptor));
            layout_bindings[i].binding            = descriptor.slot;
            layout_bindings[i].descriptorCount    = descriptor.as_array ? descriptor.array_length : 1;
            layout_bindings[i].stageFlags         = stage_flags;
            layout_bindings[i].pImmutableSamplers = nullptr;

            // bindless support
            layout_binding_flags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT; 
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flags_info = {};
        flags_info.sType                                          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
        flags_info.pNext                                          = nullptr;
        flags_info.bindingCount                                   = static_cast<uint32_t>(descriptors.size());
        flags_info.pBindingFlags                                  = layout_binding_flags.data();

        // create info
        VkDescriptorSetLayoutCreateInfo create_info = {};
        create_info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        create_info.flags                           = 0;
        create_info.pNext                           = &flags_info;
        create_info.bindingCount                    = static_cast<uint32_t>(descriptors.size());
        create_info.pBindings                       = layout_bindings.data();

        // descriptor set layout
        LC_VK_ASSERT_MSG(
            vkCreateDescriptorSetLayout(RHI_Context::device, &create_info, nullptr, reinterpret_cast<VkDescriptorSetLayout*>(&m_rhi_resource)),
            "Failed to allocate descriptor set layout");

        // name
        RHI_Device::SetResourceName(m_rhi_resource, RHI_Resource_Type::DescriptorSetLayout, m_object_name);
    }
}
