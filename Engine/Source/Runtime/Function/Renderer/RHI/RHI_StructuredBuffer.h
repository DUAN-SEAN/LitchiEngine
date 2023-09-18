
#pragma once

//= INCLUDES =================
//============================

namespace Spartan
{
    class RHI_StructuredBuffer : public LitchiRuntime::Object
    {
    public:
        RHI_StructuredBuffer(const uint32_t stride, const uint32_t element_count, const char* name);
        ~RHI_StructuredBuffer();

        void Update(void* data);
        void ResetOffset()           { m_offset = 0; }
        uint32_t GetStride()   const { return m_stride; }
        uint32_t GetOffset()   const { return m_offset; }
        void* GetRhiResource() const { return m_rhi_resource; }
        void* GetRhiUav()      const { return m_rhi_uav; }

    private:
        uint32_t m_stride        = 0;
        uint32_t m_offset        = 0;
        uint32_t m_element_count = 0;
        void* m_mapped_data      = nullptr;
        void* m_rhi_resource     = nullptr;
        void* m_rhi_uav          = nullptr;
    };
}
