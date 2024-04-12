
//= INCLUDES =======================
#include "Runtime/Core/pch.h"
#include "../RHI_Device.h"
#include "../RHI_Implementation.h"
#include "../RHI_StructuredBuffer.h"
//#include "../Rendering/Renderer.h"
//==================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_StructuredBuffer::RHI_StructuredBuffer(const uint32_t stride, const uint32_t element_count, const char* name)
    {
        m_object_name     = name;
        m_stride          = stride;
        m_element_count   = element_count;
        m_object_size_gpu = stride * element_count;

        // Calculate required alignment based on minimum device offset alignment
        size_t min_alignment = RHI_Device::PropertyGetMinStorageBufferOffsetAllignment();
        if (min_alignment > 0)
        {
            m_stride = static_cast<uint32_t>(static_cast<uint64_t>((m_stride + min_alignment - 1) & ~(min_alignment - 1)));
        }
        m_object_size_gpu = m_stride * m_element_count;

        // Define memory properties
        VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; // mappable
        // Create buffer
        RHI_Device::MemoryBufferCreate(m_rhi_resource, m_object_size_gpu, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, flags, nullptr, name);
        RHI_Device::SetResourceName(m_rhi_resource, RHI_Resource_Type::Buffer, name);

        // Get mapped data pointer
        m_mapped_data = RHI_Device::MemoryGetMappedDataFromBuffer(m_rhi_resource);

    }

    RHI_StructuredBuffer::~RHI_StructuredBuffer()
    {
        RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Buffer, m_rhi_resource);
        m_rhi_resource = nullptr;
    }

    void RHI_StructuredBuffer::Update(void* data_cpu)
    {
        LC_ASSERT_MSG(data_cpu != nullptr,                      "Invalid update data");
        LC_ASSERT_MSG(m_mapped_data != nullptr,                 "Invalid mapped data");
        LC_ASSERT_MSG(m_offset + m_stride <= m_object_size_gpu, "Out of memory");

        // advance offset
        if (first_update)
        {
            first_update = false;
        }
        else
        {
            m_offset += m_stride;
        }

        // we are using persistent mapping, so we can only copy
        memcpy(reinterpret_cast<std::byte*>(m_mapped_data) + m_offset, reinterpret_cast<std::byte*>(data_cpu), m_stride);
    }
}
