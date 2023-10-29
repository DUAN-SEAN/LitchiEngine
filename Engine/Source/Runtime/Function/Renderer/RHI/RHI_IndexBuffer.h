
#pragma once

//= INCLUDES =================
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace LitchiRuntime
{
    class RHI_IndexBuffer : public Object
    {
    public:
        RHI_IndexBuffer() = default;
        RHI_IndexBuffer(bool is_mappable, const char* name)
        {
            m_is_mappable = is_mappable;
            // m_object_name = name;
        }
        ~RHI_IndexBuffer();

        template<typename T>
        void Create(const std::vector<T>& indices)
        {
            m_stride          = sizeof(T);
            m_index_count     = static_cast<uint32_t>(indices.size());
        	m_object_size_gpu = static_cast<uint64_t>(m_stride * m_index_count);

            _create(static_cast<const void*>(indices.data()));
        }

        template<typename T>
        void Create(const T* indices, const uint32_t index_count)
        {
            m_stride          = sizeof(T);
            m_index_count     = index_count;
            m_object_size_gpu = static_cast<uint64_t>(m_stride * m_index_count);

            _create(static_cast<const void*>(indices));
        }

        template<typename T>
        void CreateDynamic(const uint32_t index_count)
        {
            m_stride          = sizeof(T);
            m_index_count     = index_count;
            m_object_size_gpu = static_cast<uint64_t>(m_stride * m_index_count);

            _create(nullptr);
        }

        void* GetMappedData()    const { return m_mapped_data; }
        void* GetRhiResource()   const { return m_rhi_resource; }
        uint32_t GetIndexCount() const { return m_index_count; }
        bool Is16Bit()           const { return sizeof(uint16_t) == m_stride; }
        bool Is32Bit()           const { return sizeof(uint32_t) == m_stride; }

    private:
        void _create(const void* indices);

        void* m_mapped_data    = nullptr;
        bool m_is_mappable     = false;
        uint32_t m_stride      = 0;
        uint32_t m_index_count = 0;

        // RHI Resources
        void* m_rhi_resource = nullptr;
    };
}
