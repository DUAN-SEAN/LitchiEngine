
#pragma once

//= INCLUDES =================
#include "RHI_Definitions.h"
#include "RHI_CommandList.h"
#include <array>
//============================

namespace LitchiRuntime
{
    const uint32_t cmd_lists_per_pool = 3;

    class RHI_CommandPool : public Object
    {
    public:
        RHI_CommandPool(const char* name, const uint64_t swap_chain_id, const RHI_Queue_Type queue_type);
        ~RHI_CommandPool();

        bool Tick();

        RHI_CommandList* GetCurrentCommandList()       { return m_using_pool_a ? m_cmd_lists_0[m_index].get() : m_cmd_lists_1[m_index].get(); }
        uint64_t GetSwapchainId()                const { return m_swap_chain_id; }

    private:
        std::array<std::shared_ptr<RHI_CommandList>, 2> m_cmd_lists_0;
        std::array<std::shared_ptr<RHI_CommandList>, 2> m_cmd_lists_1;
        std::array<void*, 2> m_rhi_resources;

        uint32_t m_index            = 0;
        bool m_using_pool_a         = true;
        bool m_first_tick           = true;
        uint64_t m_swap_chain_id    = 0;
        RHI_Queue_Type m_queue_type = RHI_Queue_Type::Max;
    };
}
