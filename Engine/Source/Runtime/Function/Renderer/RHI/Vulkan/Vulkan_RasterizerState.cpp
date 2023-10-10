
//= INCLUDES ======================
#include "Runtime/Core/pch.h"
#include "../RHI_RasterizerState.h"
//=================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_RasterizerState::RHI_RasterizerState
    (
        const RHI_CullMode cull_mode,
        const RHI_PolygonMode polygon_mode,
        const bool depth_clip_enabled,
        const bool scissor_enabled,
        const float depth_bias              /*= 0.0f */,
        const float depth_bias_clamp        /*= 0.0f */,
        const float depth_bias_slope_scaled /*= 0.0f */,
        const float line_width              /*= 1.0f */)
    {
        // save
        m_cull_mode               = cull_mode;
        m_polygon_mode            = polygon_mode;
        m_depth_clip_enabled      = depth_clip_enabled;
        m_scissor_enabled         = scissor_enabled;
        m_depth_bias              = depth_bias;
        m_depth_bias_clamp        = depth_bias_clamp;
        m_depth_bias_slope_scaled = depth_bias_slope_scaled;
        m_line_width              = line_width;

        // hash
        hash<float> hasher;
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_cull_mode));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_polygon_mode));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_depth_clip_enabled));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_scissor_enabled));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(m_line_width));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(hasher(m_depth_bias)));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(hasher(m_depth_bias_clamp)));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(hasher(m_depth_bias_slope_scaled)));
        m_hash = rhi_hash_combine(m_hash, static_cast<uint64_t>(hasher(m_line_width)));
    }
    
    RHI_RasterizerState::~RHI_RasterizerState()
    {
    
    }
}
