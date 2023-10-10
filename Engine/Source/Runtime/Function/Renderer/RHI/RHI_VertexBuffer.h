
#pragma once

//= INCLUDES ==================
#include <vector>
#include "RHI_Object.h"
//=============================

namespace LitchiRuntime
{
    class RHI_VertexBuffer : public RHI_Object
    {
    public:
        RHI_VertexBuffer() = default;
        RHI_VertexBuffer(bool is_mappable, const char* name)
        {
            m_is_mappable = is_mappable;
            // m_object_name = name;
        }
        ~RHI_VertexBuffer();

        template<typename T>
        void Create(const std::vector<T>& vertices)
        {
            m_stride          = static_cast<uint32_t>(sizeof(T));
            m_vertex_count    = static_cast<uint32_t>(vertices.size());
            m_object_size_gpu = static_cast<uint64_t>(m_stride * m_vertex_count);

            _create(static_cast<const void*>(vertices.data()));
        }

        template<typename T>
        void Create(const T* vertices, const uint32_t vertex_count)
        {
            m_stride          = static_cast<uint32_t>(sizeof(T));
            m_vertex_count    = vertex_count;
            m_object_size_gpu = static_cast<uint64_t>(m_stride * m_vertex_count);

            _create(static_cast<const void*>(vertices));
        }

        template<typename T>
        void CreateDynamic(const uint32_t vertex_count)
        {
            m_stride          = static_cast<uint32_t>(sizeof(T));
            m_vertex_count    = vertex_count;
            m_object_size_gpu = static_cast<uint64_t>(m_stride * m_vertex_count);

            _create(nullptr);
        }

        void* GetMappedData()     const { return m_mapped_data; }
        void* GetRhiResource()    const { return m_rhi_resource; }
        uint32_t GetStride()      const { return m_stride; }
        uint32_t GetVertexCount() const { return m_vertex_count; }

    private:
        void _create(const void* vertices);

        void* m_mapped_data      = nullptr;
        bool m_is_mappable       = false;
        uint32_t m_stride        = 0;
        uint32_t m_vertex_count  = 0;

        // RHI Resources
        void* m_rhi_resource = nullptr;
    };
}
