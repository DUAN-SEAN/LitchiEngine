
//= INCLUDES ========================
#include "Runtime/Core/pch.h"
#include "../RHI_DepthStencilState.h"
//===================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_DepthStencilState::RHI_DepthStencilState(
        const bool depth_test                                     /*= true*/,
        const bool depth_write                                    /*= true*/,
        const RHI_Comparison_Function depth_comparison_function   /*= Comparison_LessEqual*/,
        const bool stencil_test                                   /*= false */,
        const bool stencil_write                                  /*= false */,
        const RHI_Comparison_Function stencil_comparison_function /*= RHI_Comparison_Equal */,
        const RHI_Stencil_Operation stencil_fail_op               /*= RHI_Stencil_Keep */,
        const RHI_Stencil_Operation stencil_depth_fail_op         /*= RHI_Stencil_Keep */,
        const RHI_Stencil_Operation stencil_pass_op               /*= RHI_Stencil_Replace */
    )
    {
        // save
        m_depth_test_enabled          = depth_test;
        m_depth_write_enabled         = depth_write;
        m_depth_comparison_function   = depth_comparison_function;
        m_stencil_test_enabled        = stencil_test;
        m_stencil_write_enabled       = stencil_write;
        m_stencil_comparison_function = stencil_comparison_function;
        m_stencil_fail_op             = stencil_fail_op;
        m_stencil_depth_fail_op       = stencil_depth_fail_op;
        m_stencil_pass_op             = stencil_pass_op;

        // hash
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_depth_test_enabled));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_depth_write_enabled));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_depth_comparison_function));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_stencil_test_enabled));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_stencil_write_enabled));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_stencil_comparison_function));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_stencil_fail_op));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_stencil_depth_fail_op));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_stencil_pass_op));
    }

    RHI_DepthStencilState::~RHI_DepthStencilState() = default;
}
