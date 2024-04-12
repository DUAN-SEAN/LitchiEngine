
//= INCLUDES =======================
#include "Runtime/Core/pch.h"
#include "RHI_CommandList.h"
#include "RHI_Device.h"
#include "RHI_Fence.h"
#include "RHI_Semaphore.h"
#include "RHI_DescriptorSetLayout.h"
#include "RHI_Shader.h"
#include "RHI_Pipeline.h"
#include "RHI_CommandPool.h"
#include "RHI_Texture.h"
//#include "../Rendering/Renderer.h"
//==================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    void RHI_CommandList::WaitForExecution()
    {
        LC_ASSERT_MSG(m_state == RHI_CommandListState::Submitted, "The command list hasn't been submitted, can't wait for it.");

        // wait for execution to finish
        if (IsExecuting())
        {
            LC_ASSERT_MSG(m_rendering_complete_fence->Wait(), "Timed out while waiting for the fence");
        }

        // reset fence
        if (m_rendering_complete_fence->GetStateCpu() == RHI_Sync_State::Submitted)
        {
            m_rendering_complete_fence->Reset();
        }

        m_state = RHI_CommandListState::Idle;
    }

    bool RHI_CommandList::IsExecuting()
    {
        return
            m_state == RHI_CommandListState::Submitted && // it has been submitted
            !m_rendering_complete_fence->IsSignaled();            // and the fence is not signaled yet
    }

    void RHI_CommandList::Dispatch(RHI_Texture* texture)
    {
        const float thread_group_count = 8.0f;
        const uint32_t thread_group_count_x = static_cast<uint32_t>(Math::Helper::Ceil(static_cast<float>(texture->GetWidth()) / thread_group_count));
        const uint32_t thread_group_count_y = static_cast<uint32_t>(Math::Helper::Ceil(static_cast<float>(texture->GetHeight()) / thread_group_count));

        Dispatch(thread_group_count_x, thread_group_count_y);
    }
}
