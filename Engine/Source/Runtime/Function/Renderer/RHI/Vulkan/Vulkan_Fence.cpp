
//= INCLUDES =====================
#include "Runtime/Core/pch.h"
#include "../RHI_Fence.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
//================================

namespace LitchiRuntime
{
    RHI_Fence::RHI_Fence(const char* name /*= nullptr*/)
    {
        // Describe
        VkFenceCreateInfo fence_info = {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        // Create
        LC_ASSERT_MSG(vkCreateFence(RHI_Context::device, &fence_info, nullptr, reinterpret_cast<VkFence*>(&m_rhi_resource)) == VK_SUCCESS, "Failed to create fence");

        // Name
        if (name)
        {
            m_object_name = name;
            RHI_Device::SetResourceName(m_rhi_resource, RHI_Resource_Type::Fence, m_object_name);
        }
    }

    RHI_Fence::~RHI_Fence()
    {
        if (!m_rhi_resource)
            return;

        RHI_Device::DeletionQueueAdd(RHI_Resource_Type::Fence, m_rhi_resource);
        m_rhi_resource = nullptr;
    }

    bool RHI_Fence::IsSignaled()
    {
        return vkGetFenceStatus(RHI_Context::device, reinterpret_cast<VkFence>(m_rhi_resource)) == VK_SUCCESS;
    }

    bool RHI_Fence::Wait(uint64_t timeout_nanoseconds /*= 1000000000*/)
    {
        return vkWaitForFences(RHI_Context::device, 1, reinterpret_cast<VkFence*>(&m_rhi_resource), true, timeout_nanoseconds) == VK_SUCCESS;
    }

    void RHI_Fence::Reset()
    {
        LC_VK_ASSERT_MSG(vkResetFences(RHI_Context::device, 1, reinterpret_cast<VkFence*>(&m_rhi_resource)), "Failed to reset fence");
        n_state_cpu = RHI_Sync_State::Idle;
    }
}
