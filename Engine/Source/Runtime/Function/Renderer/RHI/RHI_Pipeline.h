
#pragma once

//= INCLUDES =================
#include <memory>
#include "RHI_PipelineState.h"
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace Spartan
{
    class SP_CLASS RHI_Pipeline : public LitchiRuntime::Object
    {
    public:
        RHI_Pipeline() = default;
        RHI_Pipeline(RHI_PipelineState& pipeline_state, RHI_DescriptorSetLayout* descriptor_set_layout);
        ~RHI_Pipeline();

        void* GetResource_Pipeline()          const { return m_resource_pipeline; }
        void* GetResource_PipelineLayout()    const { return m_resource_pipeline_layout; }
        RHI_PipelineState* GetPipelineState()       { return &m_state; }

    private:
        RHI_PipelineState m_state;
 
        // API
        void* m_resource_pipeline        = nullptr;
        void* m_resource_pipeline_layout = nullptr;
    };
}
