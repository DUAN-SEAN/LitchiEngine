
#pragma once

//= INCLUDES =================
#include <memory>
#include "RHI_Definitions.h"
#include "Runtime/Core/Meta/Reflection/Object.h"
//============================

namespace LitchiRuntime
{
    class SP_CLASS RHI_DepthStencilState : public Object
    {
    public:
        RHI_DepthStencilState(
            const bool depth_test                                     = true,
            const bool depth_write                                    = true,
            const RHI_Comparison_Function depth_comparison_function   = RHI_Comparison_Function::LessEqual,
            const bool stencil_test                                   = false,
            const bool stencil_write                                  = false,
            const RHI_Comparison_Function stencil_comparison_function = RHI_Comparison_Function::Always,
            const RHI_Stencil_Operation stencil_fail_op               = RHI_Stencil_Operation::Keep,
            const RHI_Stencil_Operation stencil_depth_fail_op         = RHI_Stencil_Operation::Keep,
            const RHI_Stencil_Operation stencil_pass_op               = RHI_Stencil_Operation::Replace
        );
        ~RHI_DepthStencilState();

        bool GetDepthTestEnabled()                             const { return m_depth_test_enabled; }
        bool GetDepthWriteEnabled()                            const { return m_depth_write_enabled; }
        bool GetStencilTestEnabled()                           const { return m_stencil_test_enabled; }
        bool GetStencilWriteEnabled()                          const { return m_stencil_write_enabled; }
        RHI_Comparison_Function GetDepthComparisonFunction()   const { return m_depth_comparison_function; }
        RHI_Comparison_Function GetStencilComparisonFunction() const { return m_stencil_comparison_function; }
        RHI_Stencil_Operation GetStencilFailOperation()        const { return m_stencil_fail_op; }
        RHI_Stencil_Operation GetStencilDepthFailOperation()   const { return m_stencil_depth_fail_op; }
        RHI_Stencil_Operation GetStencilPassOperation()        const { return m_stencil_pass_op; }
        uint8_t GetStencilReadMask()                           const { return m_stencil_read_mask; }
        uint8_t GetStencilWriteMask()                          const { return m_stencil_write_mask; }

        uint64_t GetHash()     const { return m_hash; }
        void* GetRhiResource() const { return m_rhi_resource; }

    private:
        bool m_depth_test_enabled                             = false;
        bool m_depth_write_enabled                            = false;
        RHI_Comparison_Function m_depth_comparison_function   = RHI_Comparison_Function::Never;
        bool m_stencil_test_enabled                           = false;
        bool m_stencil_write_enabled                          = false;
        RHI_Comparison_Function m_stencil_comparison_function = RHI_Comparison_Function::Never;
        RHI_Stencil_Operation m_stencil_fail_op               = RHI_Stencil_Operation::Keep;
        RHI_Stencil_Operation m_stencil_depth_fail_op         = RHI_Stencil_Operation::Keep;
        RHI_Stencil_Operation m_stencil_pass_op               = RHI_Stencil_Operation::Replace;
        uint8_t m_stencil_read_mask                           = 1;
        uint8_t m_stencil_write_mask                          = 1;

        uint64_t m_hash      = 0;
        void* m_rhi_resource = nullptr;
    };
}
