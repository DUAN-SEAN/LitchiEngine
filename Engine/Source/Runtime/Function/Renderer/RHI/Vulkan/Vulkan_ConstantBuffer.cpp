
//= INCLUDES =====================
#include "Runtime/Core/pch.h"
#include "../RHI_Implementation.h"
#include "../RHI_ConstantBuffer.h"
#include "../RHI_Device.h"
//================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
    RHI_ConstantBuffer::RHI_ConstantBuffer(const string& name)
    {
        m_object_name = name;
    }

    RHI_ConstantBuffer::~RHI_ConstantBuffer()
    {
        if (m_rhi_resource)
        {
            RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Buffer, m_rhi_resource);
            m_rhi_resource = nullptr;
        }
    }

    void RHI_ConstantBuffer::RHI_CreateResource()
    {
        // destroy previous buffer
        if (m_rhi_resource)
        {
            RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Buffer, m_rhi_resource);
            m_rhi_resource = nullptr;
        }

        // calculate required alignment based on minimum device offset alignment
        size_t min_alignment = RHI_Device::PropertyGetMinUniformBufferOffsetAllignment();
        if (min_alignment > 0)
        {
            m_stride = static_cast<uint32_t>(static_cast<uint64_t>((m_stride + min_alignment - 1) & ~(min_alignment - 1)));
        }
        m_object_size_gpu = m_stride * m_element_count;

        // define memory properties
        VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; // mappable

        // create buffer
        RHI_Device::MemoryBufferCreate(m_rhi_resource, m_object_size_gpu, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, flags, nullptr, m_object_name.c_str());

        // get mapped data pointer
        m_mapped_data = RHI_Device::MemoryGetMappedDataFromBuffer(m_rhi_resource);

        // set debug name
        RHI_Device::SetResourceName(m_rhi_resource, RHI_Resource_Type::Buffer, m_object_name);
    }

    void RHI_ConstantBuffer::Update(void* data_cpu)
    {
        SP_ASSERT_MSG(data_cpu != nullptr,                      "Invalid update data");
        SP_ASSERT_MSG(m_mapped_data != nullptr,                 "Invalid mapped data");
        SP_ASSERT_MSG(m_offset + m_stride <= m_object_size_gpu, "Out of memory");

        // advance offset
        if (m_has_updated)
        {
            m_offset += m_stride;
        }

        // we are using persistent mapping, so we can only copy
        memcpy(reinterpret_cast<std::byte*>(m_mapped_data) + m_offset, reinterpret_cast<std::byte*>(data_cpu), m_stride);

        m_has_updated = true;
    }
}
