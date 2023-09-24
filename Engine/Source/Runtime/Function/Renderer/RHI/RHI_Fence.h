
#pragma once

//= INCLUDES =================
#include "RHI_Definitions.h"
#include "RHI_Object.h"
//============================

namespace Spartan
{
    class SP_CLASS RHI_Fence : public RHI_Object
    {
    public:
        RHI_Fence(const char* name = nullptr);
        ~RHI_Fence();

        // Returns true if the false was signaled.
        bool IsSignaled();

        // Returns true when the fence is signaled and false in case of a timeout.
        bool Wait(uint64_t timeout_nanoseconds = 1000000000 /* one second */);

        // Resets the fence
        void Reset();

        void* GetRhiResource() { return m_rhi_resource; }

        // State
        RHI_Sync_State GetStateCpu()                 const { return n_state_cpu; }
        void SetStateCpu(const RHI_Sync_State state)       { n_state_cpu = state; }

    private:
        void* m_rhi_resource           = nullptr;
        RHI_Sync_State n_state_cpu = RHI_Sync_State::Idle;
    };
}
