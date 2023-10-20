
#pragma once

//= INCLUDES =================
#include "RHI_Definitions.h"
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace LitchiRuntime
{
    class RHI_Semaphore : public Object
    {
    public:
        RHI_Semaphore(bool is_timeline = false, const char* name = nullptr);
        ~RHI_Semaphore();

        // Timeline
        bool IsTimelineSemaphore() const { return m_is_timeline; }
        void Wait(const uint64_t value, const uint64_t timeout = std::numeric_limits<uint64_t>::max());
        void Signal(const uint64_t value);
        uint64_t GetValue();
        void* GetRhiResource() { return m_rhi_resource; }

        // State
        RHI_Sync_State GetStateCpu()                 const { return m_state_cpu; }
        void SetStateCpu(const RHI_Sync_State state)       { m_state_cpu = state; }

    private:
        void* m_rhi_resource           = nullptr;
        bool m_is_timeline         = false;
        RHI_Sync_State m_state_cpu = RHI_Sync_State::Idle;
    };
}
