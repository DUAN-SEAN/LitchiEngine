
//= INCLUDES =====================
#include "Runtime/Core/pch.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_CommandPool.h"
//================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_CommandPool::RHI_CommandPool(const char* name, const uint64_t swap_chain_id, const RHI_Queue_Type queue_type) : Object()
    {
        m_object_name   = name;
        m_swap_chain_id = swap_chain_id;
        m_queue_type    = queue_type;


        // create command pools
        {
            VkCommandPoolCreateInfo cmd_pool_info = {};
            cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmd_pool_info.queueFamilyIndex = RHI_Device::QueueGetIndex(queue_type);
            cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // specifies that command buffers allocated from the pool will be short-lived

            // create the first one
            VkCommandPool cmd_pool = nullptr;
            LC_VK_ASSERT_MSG(vkCreateCommandPool(RHI_Context::device, &cmd_pool_info, nullptr, &cmd_pool), "Failed to create command pool");
            RHI_Device::SetResourceName(cmd_pool, RHI_Resource_Type::CommandPool, m_object_name + string("_0"));
            m_rhi_resources[0] = static_cast<void*>(cmd_pool);

            // create the second one
            cmd_pool = nullptr;
            LC_VK_ASSERT_MSG(vkCreateCommandPool(RHI_Context::device, &cmd_pool_info, nullptr, &cmd_pool), "Failed to create command pool");
            RHI_Device::SetResourceName(cmd_pool, RHI_Resource_Type::CommandPool, m_object_name + string("_1"));
            m_rhi_resources[1] = static_cast<void*>(cmd_pool);
        }

        // create command lists
        for (uint32_t i = 0; i < cmd_lists_per_pool; i++)
        {
            string name = m_object_name + "_cmd_pool_0_" + to_string(0);
            m_cmd_lists_0[i] = make_shared<RHI_CommandList>(queue_type, m_swap_chain_id, m_rhi_resources[0], name.c_str());

            name = m_object_name + "_cmd_pool_1_" + to_string(0);
            m_cmd_lists_1[i] = make_shared<RHI_CommandList>(queue_type, m_swap_chain_id, m_rhi_resources[1], name.c_str());
        }
    }

    RHI_CommandPool::~RHI_CommandPool()
    {
        // wait for GPU
        RHI_Device::QueueWait(m_queue_type);

        // free command lists
        for (uint32_t i = 0; i < cmd_lists_per_pool; i++)
        {
            VkCommandBuffer vk_cmd_buffer = reinterpret_cast<VkCommandBuffer>(m_cmd_lists_0[i]->GetRhiResource());
            vkFreeCommandBuffers(
                RHI_Context::device,
                static_cast<VkCommandPool>(m_rhi_resources[0]),
                1,
                &vk_cmd_buffer
            );

            vk_cmd_buffer = reinterpret_cast<VkCommandBuffer>(m_cmd_lists_1[i]->GetRhiResource());
            vkFreeCommandBuffers(
                RHI_Context::device,
                static_cast<VkCommandPool>(m_rhi_resources[1]),
                1,
                &vk_cmd_buffer
            );
        }

        // destroy commend pools
        vkDestroyCommandPool(RHI_Context::device, static_cast<VkCommandPool>(m_rhi_resources[0]), nullptr);
        vkDestroyCommandPool(RHI_Context::device, static_cast<VkCommandPool>(m_rhi_resources[1]), nullptr);
    }

    bool RHI_CommandPool::Tick()
    {
        bool has_been_reset = false;

        if (m_first_tick)
        {
            m_first_tick = false;
            return has_been_reset;
        }

        m_index++;

        // if we have no more command lists, switch to the other pool
        if (m_index == cmd_lists_per_pool)
        {
            // switch command pool
            m_index = 0;
            m_using_pool_a = !m_using_pool_a;
            auto& cmd_lists = m_using_pool_a ? m_cmd_lists_0 : m_cmd_lists_1;
            VkCommandPool pool = static_cast<VkCommandPool>(m_rhi_resources[m_using_pool_a ? 0 : 1]);

            // wait
            for (shared_ptr<RHI_CommandList> cmd_list : cmd_lists)
            {
                LC_ASSERT(cmd_list->GetState() != RHI_CommandListState::Recording);

                if (cmd_list->GetState() == RHI_CommandListState::Submitted)
                {
                    cmd_list->WaitForExecution();
                }
            }

            // reset
            LC_VK_ASSERT_MSG(vkResetCommandPool(RHI_Context::device, pool, 0), "Failed to reset command pool");
            has_been_reset = true;
        }

        return has_been_reset;
    }
}
