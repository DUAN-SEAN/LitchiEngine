
#pragma once

//= INCLUDES =================
#include <memory>

#include "Runtime/Core/Meta/Reflection/object.h"

//============================

namespace LitchiRuntime
{
    class SP_CLASS RHI_ConstantBuffer : public Object
    {
    public:
        RHI_ConstantBuffer() = default;
        RHI_ConstantBuffer(const std::string& name);
        ~RHI_ConstantBuffer();

        template<typename T>
        void Create(const uint32_t element_count)
        {
            SP_ASSERT_STATIC_IS_TRIVIALLY_COPYABLE(T);
            SP_ASSERT_MSG(sizeof(T) % 16 == 0, "The size is not a multiple of 16");
            SP_ASSERT_MSG(element_count != 0,  "Element count can't be zero");

            m_type_size     = static_cast<uint32_t>(sizeof(T));
            m_stride        = m_type_size; // will be aligned based on minimum device offset alignment
            m_element_count = element_count;

            RHI_CreateResource();
        }

        void Update(void* data_cpu);

        void ResetOffset()
        {
            m_offset      = 0;
            m_has_updated = false;
        }

        uint32_t GetStructSize()  const { return m_type_size; }
        uint32_t GetStride()      const { return m_stride; }
        uint32_t GetOffset()      const { return m_offset; }
        uint32_t GetStrideCount() const { return m_element_count; }
        void* GetRhiResource()    const { return m_rhi_resource; }

    private:
        void RHI_CreateResource();

        uint32_t m_type_size     = 0;
        uint32_t m_stride        = 0;
        uint32_t m_offset        = 0;
        uint32_t m_element_count = 0;
        bool m_has_updated       = false;
        void* m_mapped_data      = nullptr;
        void* m_rhi_resource     = nullptr;
    };
}
