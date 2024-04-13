
#pragma once

//= INCLUDES =================================
#include <array>
#include <atomic>
#include <mutex>

#include "RHI_Definitions.h"
#include "RHI_PipelineState.h"
#include "RHI_Descriptor.h"
#include "Runtime/Core/Meta/Reflection/object.h"
#include "Runtime/Function/Renderer/Rendering/Renderer_Definitions.h"
//============================================

namespace LitchiRuntime
{
    // forward declarations
    class Rectangle;
    enum class RHI_CommandListState : uint8_t
    {
        Idle,
        Recording,
        Ended,
        Submitted
    };

    class LC_CLASS RHI_CommandList : public Object
    {
    public:
        RHI_CommandList(const RHI_Queue_Type queue_type, const uint32_t swapchain_id, void* cmd_pool_resource, const char* name);
        ~RHI_CommandList();

        void Begin();
        void End();
        void Submit();
        void WaitForExecution();

        // Render pass
        void SetPipelineState(RHI_PipelineState& pso);
        // Render pass
        void SetPipelineState(RHI_PipelineState& pso, bool needBeginRenderPass);

        // Clear
        void ClearPipelineStateRenderTargets(RHI_PipelineState& pipeline_state);
        void ClearRenderTarget(
            RHI_Texture* texture,
            const uint32_t color_index         = 0,
            const uint32_t depth_stencil_index = 0,
            const bool storage                 = false,
            const Color& clear_color           = rhi_color_load,
            const float clear_depth            = rhi_depth_load,
            const uint32_t clear_stencil       = rhi_stencil_load
        );

        // Draw
        void Draw(uint32_t vertex_count, uint32_t vertex_start_index = 0);
        void DrawIndexed(const uint32_t index_count, const uint32_t index_offset = 0, const uint32_t vertex_offset = 0, const uint32_t instance_start_index = 0, const uint32_t instance_count = 1);

        // Dispatch
        void Dispatch(uint32_t x, uint32_t y, uint32_t z = 1, bool async = false);
        void Dispatch(RHI_Texture* texture);

        // blit
        void Blit(RHI_Texture* source, RHI_Texture* destination, const bool blit_mips, const float source_scaling = 1.0f);
        void Blit(RHI_Texture* source, RHI_SwapChain* destination);

        // Copy
        void Copy(RHI_Texture* source, RHI_Texture* destination, const bool blit_mips);
        void Copy(RHI_Texture* source, RHI_SwapChain* destination);

        // Viewport
        void SetViewport(const RHI_Viewport& viewport) const;
        
        // Scissor
        void SetScissorRectangle(const Rectangle& scissor_rectangle) const;

        // cull mode
        void SetCullMode(const RHI_CullMode cull_mode);

        // Vertex buffer
        void SetBufferVertex(const RHI_VertexBuffer* buffer, const uint32_t binding = 0);
        
        // Index buffer
        void SetBufferIndex(const RHI_IndexBuffer* buffer);

        // Constant buffer
        void SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constant_buffer) const;
        void SetConstantBuffer(const Renderer_BindingsCb slot, const std::shared_ptr<RHI_ConstantBuffer>& constant_buffer) const { SetConstantBuffer(static_cast<uint32_t>(slot), constant_buffer.get()); }

        // Push constant buffer
        void PushConstants(const uint32_t offset, const uint32_t size, const void* data);

        // Sampler
        void SetSampler(const uint32_t slot, RHI_Sampler* sampler) const;
        void SetSampler(const uint32_t slot, const std::shared_ptr<RHI_Sampler>& sampler) const { SetSampler(slot, sampler.get()); }

        // Texture
        void SetTexture(const uint32_t slot, RHI_Texture* texture, const uint32_t mip_index = rhi_all_mips, uint32_t mip_range = 0, const bool uav = false);
        void SetTexture(const Renderer_BindingsUav slot,                       RHI_Texture* texture,  const uint32_t mip_index = rhi_all_mips, uint32_t mip_range = 0) { SetTexture(static_cast<uint32_t>(slot), texture,       mip_index, mip_range, true); }
        void SetTexture(const Renderer_BindingsUav slot, const std::shared_ptr<RHI_Texture>& texture, const uint32_t mip_index = rhi_all_mips, uint32_t mip_range = 0) { SetTexture(static_cast<uint32_t>(slot), texture.get(), mip_index, mip_range, true); }
        void SetTexture(const Renderer_BindingsSrv slot,                       RHI_Texture* texture,  const uint32_t mip_index = rhi_all_mips, uint32_t mip_range = 0) { SetTexture(static_cast<uint32_t>(slot), texture,       mip_index, mip_range, false); }
        void SetTexture(const Renderer_BindingsSrv slot, const std::shared_ptr<RHI_Texture>& texture, const uint32_t mip_index = rhi_all_mips, uint32_t mip_range = 0) { SetTexture(static_cast<uint32_t>(slot), texture.get(), mip_index, mip_range, false); }

        // Structured buffer
        void SetStructuredBuffer(const uint32_t slot, RHI_StructuredBuffer* structured_buffer) const;
        void SetStructuredBuffer(const Renderer_BindingsUav slot, const std::shared_ptr<RHI_StructuredBuffer>& structured_buffer) const { SetStructuredBuffer(static_cast<uint32_t>(slot), structured_buffer.get()); }

        // Material Global Buffer
        void SetMaterialGlobalBuffer(RHI_ConstantBuffer* constant_buffer) const;

        // Markers
        void BeginMarker(const char* name);
        void EndMarker();

        // Timestamps
        uint32_t BeginTimestamp();
        void EndTimestamp();
        float GetTimestampResult(const uint32_t index_timestamp);

        // occlusion queries
        void BeginOcclusionQuery(const uint64_t entity_id);
        void EndOcclusionQuery();
        bool GetOcclusionQueryResult(const uint64_t entity_id);
        void UpdateOcclusionQueries();

        // Timeblocks (Markers + Timestamps)
        void BeginTimeblock(const char* name, const bool gpu_marker = true, const bool gpu_timing = true);
        void EndTimeblock();

        // State
        const RHI_CommandListState GetState() const { return m_state; }
        bool IsExecuting();

        // memory barriers
        void InsertBarrierTexture(void* image, const uint32_t aspect_mask, const uint32_t mip_index, const uint32_t mip_range, const uint32_t array_length, const RHI_Image_Layout layout_old, const RHI_Image_Layout layout_new, const bool is_depth);
        void InsertBarrierTexture(RHI_Texture* texture, const uint32_t mip_start, const uint32_t mip_range, const uint32_t array_length, const RHI_Image_Layout layout_old, const RHI_Image_Layout layout_new);
        void InsertBarrierTextureReadWrite(RHI_Texture* texture);

        // Misc
        RHI_Semaphore* GetSemaphoreProccessed() { return m_rendering_complete_semaphore.get(); }
        void* GetRhiResource() const { return m_rhi_resource; }

    private:
        //void OnDraw();
        //void GetDescriptorsFromPipelineState(RHI_PipelineState& pipeline_state, std::vector<RHI_Descriptor>& descriptors);

        void RenderPassBegin();
        void RenderPassEnd();

        // sync
        std::shared_ptr<RHI_Fence> m_rendering_complete_fence;
        std::shared_ptr<RHI_Semaphore> m_rendering_complete_semaphore;

        // variables to minimise state changes
        uint64_t m_vertex_buffer_id = 0;
        uint64_t m_index_buffer_id = 0;

        // misc
        uint32_t m_timestamp_index = 0;
        RHI_Pipeline* m_pipeline = nullptr;
        bool m_render_pass_active = false;
        RHI_DescriptorSetLayout* m_descriptor_layout_current = nullptr;
        std::atomic<RHI_CommandListState> m_state = RHI_CommandListState::Idle;
        RHI_Queue_Type m_queue_type = RHI_Queue_Type::Max;
        const char* m_timeblock_active = nullptr;
        RHI_CullMode m_cull_mode = RHI_CullMode::Max;
        static bool m_memory_query_support;
        std::mutex m_mutex_reset;
        RHI_PipelineState m_pso;

        // rhi resources
        void* m_rhi_resource = nullptr;
        void* m_rhi_cmd_pool_resource = nullptr;
        void* m_rhi_query_pool_timestamps = nullptr;
        void* m_rhi_query_pool_occlusion = nullptr;
    };
}
