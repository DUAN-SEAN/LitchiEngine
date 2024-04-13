
#pragma once

//= INCLUDES =================
#include "Runtime/Core/Meta/Reflection/object.h"

//============================

namespace LitchiRuntime
{
    class RHI_StructuredBuffer : public Object
    {
    public:
        RHI_StructuredBuffer(const uint32_t stride, const uint32_t element_count, const char* name);
        ~RHI_StructuredBuffer();

        void Update(void* data, const uint32_t update_size = 0);
        void ResetOffset()           { m_offset = 0; first_update = true;}
        uint32_t GetStride()   const { return m_stride; }
        uint32_t GetOffset()   const { return m_offset; }
        void* GetRhiResource() const { return m_rhi_resource; }

    private:
        uint32_t m_stride        = 0;
        uint32_t m_offset        = 0;
        uint32_t m_element_count = 0;
        bool first_update = true;
        void* m_mapped_data      = nullptr;
        void* m_rhi_resource     = nullptr;
    };
}
