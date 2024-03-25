
#pragma once

//= INCLUDES =================
#include <memory>
#include <vector>
#include "RHI_Definitions.h"
#include "Runtime/Core/Meta/Reflection/object.h"
#include "RHI_Vertex.h"
//============================

namespace LitchiRuntime
{
    struct VertexAttribute 
    {
        VertexAttribute(const std::string& name, const uint32_t location, const uint32_t binding, const RHI_Format format, const uint32_t offset)
        {
            this->name     = name;
            this->location = location;
            this->binding  = binding;
            this->format   = format;
            this->offset   = offset;
        }

        std::string name;
        uint32_t location;
        uint32_t binding;
        RHI_Format format;
        uint32_t offset;
    };

    class LC_CLASS RHI_InputLayout : public Object
    {
    public:
        RHI_InputLayout() = default;
        ~RHI_InputLayout();

        void Create(const RHI_Vertex_Type vertex_type, void* vertex_shader_blob = nullptr)
        {
            m_vertex_type = vertex_type;
            const uint32_t binding = 0;

            if (vertex_type == RHI_Vertex_Type::Undefined)
            {
                // The full-screen triangle vertex shaders generates it's own vertices.
                // Therefore it doesn't need to define a vertex type for an input layout.
                m_vertex_size = 0;
            }
            else if (vertex_type == RHI_Vertex_Type::Pos)
            {
                m_vertex_attributes =
                {
                    { "POSITION", 0, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_Pos, pos) }
                };

                m_vertex_size = sizeof(RHI_Vertex_Pos);
            }
            else if (vertex_type == RHI_Vertex_Type::PosUv)
            {
                m_vertex_attributes =
                {
                    { "POSITION", 0, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTex, pos) },
                    { "TEXCOORD", 1, binding, RHI_Format::R32G32_Float,    offsetof(RHI_Vertex_PosTex, tex) }
                };

                m_vertex_size = sizeof(RHI_Vertex_PosTex);
            }
            else if (vertex_type == RHI_Vertex_Type::PosCol)
            {
                m_vertex_attributes =
                {
                    { "POSITION", 0, binding, RHI_Format::R32G32B32_Float,    offsetof(RHI_Vertex_PosCol, pos) },
                    { "COLOR",    1, binding, RHI_Format::R32G32B32A32_Float, offsetof(RHI_Vertex_PosCol, col) }
                };

                m_vertex_size = sizeof(RHI_Vertex_PosCol);
            }
            else if (vertex_type == RHI_Vertex_Type::Pos2dUvCol8)
            {
                m_vertex_attributes =
                {
                    { "POSITION", 0, binding, RHI_Format::R32G32_Float,   offsetof(RHI_Vertex_Pos2dTexCol8, pos) },
                    { "TEXCOORD", 1, binding, RHI_Format::R32G32_Float,   offsetof(RHI_Vertex_Pos2dTexCol8, tex) },
                    { "COLOR",    2, binding, RHI_Format::R8G8B8A8_Unorm, offsetof(RHI_Vertex_Pos2dTexCol8, col) }
                };

                m_vertex_size = sizeof(RHI_Vertex_Pos2dTexCol8);
            }
            else if (vertex_type == RHI_Vertex_Type::PosUvNorTan)
            {
                m_vertex_attributes =
                {
                    { "POSITION", 0, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTexNorTan, pos) },
                    { "TEXCOORD", 1, binding, RHI_Format::R32G32_Float,    offsetof(RHI_Vertex_PosTexNorTan, tex) },
                    { "NORMAL",   2, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTexNorTan, nor) },
                    { "TANGENT",  3, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTexNorTan, tan) }
                };

                m_vertex_size = sizeof(RHI_Vertex_PosTexNorTan);
            }
        	else if(vertex_type == RHI_Vertex_Type::PosUvNorTanBone)
            {
                m_vertex_attributes =
                {
                    { "POSITION", 0, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTexNorTanBone, pos) },
                    { "TEXCOORD", 1, binding, RHI_Format::R32G32_Float,    offsetof(RHI_Vertex_PosTexNorTanBone, tex) },
                    { "NORMAL",   2, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTexNorTanBone, nor) },
                    { "TANGENT",  3, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTexNorTanBone, tan) },
                    { "BONEINDICES",  4, binding, RHI_Format::R32G32B32A32_UINT, offsetof(RHI_Vertex_PosTexNorTanBone, boneIndices) },
                    { "BONEWEIGHTS",  5, binding, RHI_Format::R32G32B32_Float, offsetof(RHI_Vertex_PosTexNorTanBone, boneWeights) }
                };
                m_vertex_size = sizeof(RHI_Vertex_PosTexNorTanBone);
            }
        }

        RHI_Vertex_Type GetVertexType()                                const { return m_vertex_type; }
        const uint32_t GetVertexSize()                                 const { return m_vertex_size; }
        const std::vector<VertexAttribute>& GetAttributeDescriptions() const { return m_vertex_attributes; }
        uint32_t GetAttributeCount()                                   const { return static_cast<uint32_t>(m_vertex_attributes.size()); }

        bool operator==(const RHI_InputLayout& rhs) const { return m_vertex_type == rhs.GetVertexType(); }

        void* GetRhiResource() const { return m_rhi_resource; }

    private:
        RHI_Vertex_Type m_vertex_type;
        uint32_t m_vertex_size;
        bool _CreateResource(void* vertex_shader_blob);
        std::vector<VertexAttribute> m_vertex_attributes;

        // RHI Resource
        void* m_rhi_resource = nullptr;
    };
}
