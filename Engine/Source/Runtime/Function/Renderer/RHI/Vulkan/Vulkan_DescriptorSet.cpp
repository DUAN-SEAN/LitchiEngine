
//= INCLUDES ==========================
#include "Runtime/Core/pch.h"
#include "../RHI_Device.h"
#include "../RHI_DescriptorSet.h"
#include "../RHI_Implementation.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_Sampler.h"
#include "../RHI_ConstantBuffer.h"
#include "../RHI_StructuredBuffer.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
#include "Runtime/Function/Renderer/Rendering/Renderer_Definitions.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
//=====================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    void RHI_DescriptorSet::Update(const vector<RHI_Descriptor>& descriptors)
    {
        EASY_FUNCTION(profiler::colors::Yellow500)

        m_descriptors = descriptors;

        // Validate descriptor set
        LC_ASSERT(m_resource != nullptr);

        static const uint32_t descriptor_count = 256;
        static array<VkWriteDescriptorSet, descriptor_count> descriptor_sets;

        vector<VkDescriptorImageInfo> info_images;
        info_images.resize(descriptor_count);
        info_images.reserve(descriptor_count);
        int image_index = -1;

        vector<VkDescriptorBufferInfo> info_buffers;
        info_buffers.resize(descriptor_count);
        info_buffers.reserve(descriptor_count);

        uint32_t index = 0;
        descriptor_sets = {};
        for (const RHI_Descriptor& descriptor : descriptors)
        {
            // in case of a null texture (which is legal), don't skip it
             // set a checkeboard texture instead, this way if it's sampled (which is wrong), we'll see it
            if (!descriptor.data && descriptor.type != RHI_Descriptor_Type::Texture)
                continue;

            // the binldess texture array has it's own descriptor
            bool binldess_array = descriptor.as_array && descriptor.array_length == rhi_max_array_size;
            if (binldess_array)
                continue;

            uint32_t descriptor_index_start = 0;
            uint32_t descriptor_count       = 1;

            if (descriptor.type == RHI_Descriptor_Type::Sampler)
            {
                image_index++;

                info_images[image_index].sampler     = static_cast<VkSampler>(static_cast<RHI_Sampler*>(descriptor.data)->GetRhiResource());
                info_images[image_index].imageView   = nullptr;
                info_images[image_index].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

                descriptor_index_start = image_index;
            }
            else if (descriptor.type == RHI_Descriptor_Type::Texture || descriptor.type == RHI_Descriptor_Type::TextureStorage)
            {
                RHI_Texture* texture     = static_cast<RHI_Texture*>(descriptor.data);
                const bool mip_specified = descriptor.mip != rhi_all_mips;
                uint32_t mip_start       = mip_specified ? descriptor.mip : 0;

                // get texture, if unable to do so, fallback to a checkerboard texture, so we can spot it by eye
                void* srv_fallback = Renderer::GetStandardTexture(Renderer_StandardTexture::Checkerboard)->GetRhiSrv();

                if (!descriptor.as_array)
                {
                    image_index++;

                    // get texture, if unable to do so, fallback to a checkerboard texture, so we can spot it by eye
                    void* resource = texture ? (mip_specified ? texture->GetRhiSrvMip(descriptor.mip) : texture->GetRhiSrv()) : nullptr;
                    RHI_Image_Layout layout = texture ? texture->GetLayout(mip_start) : RHI_Image_Layout::Max;
                    if (descriptor.type == RHI_Descriptor_Type::Texture && descriptor.data == nullptr)
                    {
                        resource = srv_fallback;
                        layout = RHI_Image_Layout::Shader_Read;
                    }

                    info_images[image_index].sampler = nullptr;
                    info_images[image_index].imageView = static_cast<VkImageView>(resource);
                    info_images[image_index].imageLayout = vulkan_image_layout[static_cast<uint8_t>(layout)];

                    descriptor_index_start = image_index;
                }
                else // bind mips as an array of textures (not a Texture2DArray)
                {
                    for (uint32_t mip_index = mip_start; mip_index < mip_start + descriptor.mip_range; mip_index++)
                    {
                        image_index++;


                        // get texture, if unable to do so, fallback to a checkerboard texture, so we can spot it by eye
                        void* resource = texture ? texture->GetRhiSrvMip(mip_index) : nullptr;
                        RHI_Image_Layout layout = texture ? texture->GetLayout(mip_index) : RHI_Image_Layout::Max;
                        if (descriptor.type == RHI_Descriptor_Type::Texture && descriptor.data == nullptr)
                        {
                            resource = srv_fallback;
                            layout = RHI_Image_Layout::Shader_Read;
                        }

                        info_images[image_index].sampler = nullptr;
                        info_images[image_index].imageView = static_cast<VkImageView>(resource);
                        info_images[image_index].imageLayout = vulkan_image_layout[static_cast<uint8_t>(layout)];

                        if (mip_index == descriptor.mip)
                        {
                            descriptor_index_start = image_index;
                        }
                    }

                    descriptor_count = descriptor.mip_range;
                }
            }
            else if (descriptor.type == RHI_Descriptor_Type::ConstantBuffer)
            {
                info_buffers[index].buffer = static_cast<VkBuffer>(static_cast<RHI_ConstantBuffer*>(descriptor.data)->GetRhiResource());
                info_buffers[index].offset = 0;
                info_buffers[index].range  = descriptor.range;

                descriptor_index_start = index;
            }
            else if (descriptor.type == RHI_Descriptor_Type::StructuredBuffer)
            {
                info_buffers[index].buffer = static_cast<VkBuffer>(static_cast<RHI_StructuredBuffer*>(descriptor.data)->GetRhiResource());
                info_buffers[index].offset = 0;
                info_buffers[index].range  = descriptor.range;

                descriptor_index_start = index;
            }
            else
            {
                LC_ASSERT_MSG(false, "Unhandled descriptor type");
            }

            // Write descriptor set
            descriptor_sets[index].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_sets[index].pNext            = nullptr;
            descriptor_sets[index].dstSet           = static_cast<VkDescriptorSet>(m_resource);
            descriptor_sets[index].dstBinding       = descriptor.slot;
            descriptor_sets[index].dstArrayElement  = 0; // The starting element in that array
            descriptor_sets[index].descriptorCount  = descriptor_count;
            descriptor_sets[index].descriptorType   = static_cast<VkDescriptorType>(RHI_Device::GetDescriptorType(descriptor));
            descriptor_sets[index].pImageInfo       = &info_images[descriptor_index_start];
            descriptor_sets[index].pBufferInfo      = &info_buffers[descriptor_index_start];
            descriptor_sets[index].pTexelBufferView = nullptr;

            LC_ASSERT(descriptor_sets[index].dstSet != nullptr);

            index++;
        }

        vkUpdateDescriptorSets(
            RHI_Context::device,    // device
            index,                  // descriptorWriteCount
            descriptor_sets.data(), // pDescriptorWrites
            0,                      // descriptorCopyCount
            nullptr                 // pDescriptorCopies
        );
    }
}
