
//= INCLUDES =====================
#include  "Runtime/Core/pch.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_VertexBuffer.h"
#include "../RHI_Vertex.h"
#include "../RHI_CommandList.h"
//================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
    RHI_VertexBuffer::~RHI_VertexBuffer()
    {
        if (m_rhi_resource)
        {
            RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Buffer, m_rhi_resource);
            m_rhi_resource = nullptr;
        }
    }

    void RHI_VertexBuffer::_create(const void* vertices)
    {
        // Destroy previous buffer
        if (m_rhi_resource)
        {
            RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Buffer, m_rhi_resource);
            m_rhi_resource = nullptr;
        }

        m_is_mappable = vertices == nullptr;

        if (m_is_mappable)
        {
            // Define memory properties
            uint32_t flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; // mappable

            // Created
            RHI_Device::MemoryBufferCreate(m_rhi_resource, m_object_size_gpu, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, flags, nullptr, m_object_name.c_str());

            // Get mapped data pointer
            m_mapped_data = RHI_Device::MemoryGetMappedDataFromBuffer(m_rhi_resource);
        }
        else // The reason we use staging is because memory with VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, the buffer is not not mappable but it's fast, we want that.
        {
            // Create staging/source buffer and copy the vertices to it
            void* staging_buffer = nullptr;
            RHI_Device::MemoryBufferCreate(staging_buffer, m_object_size_gpu, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices, m_object_name.c_str());

            // Create destination buffer
            RHI_Device::MemoryBufferCreate(m_rhi_resource, m_object_size_gpu, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr, m_object_name.c_str());

            // Copy staging buffer to destination buffer
            {
                // Create command buffer
                RHI_CommandList* cmd_list = RHI_Device::CmdImmediateBegin(RHI_Queue_Type::Copy);

                VkBuffer* buffer_vk         = reinterpret_cast<VkBuffer*>(&m_rhi_resource);
                VkBuffer* buffer_staging_vk = reinterpret_cast<VkBuffer*>(&staging_buffer);

                // Copy
                VkBufferCopy copy_region = {};
                copy_region.size         = m_object_size_gpu;
                vkCmdCopyBuffer(static_cast<VkCommandBuffer>(cmd_list->GetRhiResource()), *buffer_staging_vk, *buffer_vk, 1, &copy_region);

                // Flush and free command buffer
                RHI_Device::CmdImmediateSubmit(cmd_list);

                // Destroy staging resources
                RHI_Device::MemoryBufferDestroy(staging_buffer);
            }
        }

        // Set debug name
        RHI_Device::SetResourceName(m_rhi_resource, RHI_Resource_Type::Buffer, m_object_name);
    }
}
