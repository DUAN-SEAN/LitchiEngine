
//= INCLUDES ==================================
#include "Runtime/Core/pch.h"
#include "Font.h"
#include "../Renderer.h"
#include "../../RHI/RHI_Vertex.h"
#include "../../RHI/RHI_VertexBuffer.h"
#include "../../RHI/RHI_IndexBuffer.h"
#include "../../Resource/ResourceCache.h"
#include "../../Resource/Import/FontImporter.h"
#include "Runtime/Core/Tools/Utils/Stopwatch.h"
//=============================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

#define ASCII_TAB       9
#define ASCII_NEW_LINE  10
#define ASCII_SPACE     32

namespace LitchiRuntime
{
    Font::Font(const string& file_path, const uint32_t font_size, const Vector4& color) : IResource(ResourceType::Font)
    {
        m_vertex_buffer   = make_shared<RHI_VertexBuffer>(true, "font");
        m_index_buffer    = make_shared<RHI_IndexBuffer>(true, "font");
        m_char_max_width  = 0;
        m_char_max_height = 0;
        m_color           = color;
        
        SetSize(font_size);
        LoadFromFile(file_path);
    }

    bool Font::SaveToFile(const string& file_path)
    {
        return true;
    }

    bool Font::LoadFromFile(const string& file_path)
    {
        const Stopwatch timer;

        // Load
        if (!FontImporter::LoadFromFile(this, file_path))
        {
            DEBUG_LOG_ERROR("Failed to load font \"%s\"", file_path.c_str());
            return false;
        }

        // Find max character height (todo, actually get spacing from FreeType)
        for (const auto& char_info : m_glyphs)
        {
            m_char_max_width    = Helper::Max<int>(char_info.second.width, m_char_max_width);
            m_char_max_height   = Helper::Max<int>(char_info.second.height, m_char_max_height);
        }
        
        DEBUG_LOG_INFO("Loading \"%s\" took %d ms", FileSystem::GetFileNameFromFilePath(file_path).c_str(), static_cast<int>(timer.GetElapsedTimeMs()));
        return true;
    }

    void Font::SetText(const string& text, const Vector2& position)
    {
        const bool same_text   = text == m_current_text;
        const bool has_buffers = (m_vertex_buffer && m_index_buffer);

        if (same_text || !has_buffers)
            return;

        Vector2 pen = position;
        m_current_text = text;
        m_vertices.clear();

        // Draw each letter onto a quad.
        for (auto text_char : m_current_text)
        {
            Glyph& glyph = m_glyphs[text_char];

            if (text_char == ASCII_TAB)
            {
                const uint32_t space_offset      = m_glyphs[ASCII_SPACE].horizontal_advance;
                const uint32_t space_count       = 4; // spaces in a typical editor
                const uint32_t tab_spacing       = space_offset * space_count;
                const uint32_t offset_from_start = static_cast<uint32_t>(Math::Helper::Abs(pen.x - position.x));
                const uint32_t next_column_index = (offset_from_start / tab_spacing) + 1;
                const uint32_t offset_to_column  = (next_column_index * tab_spacing) - offset_from_start;
                pen.x                            += offset_to_column;
            }
            else if (text_char == ASCII_NEW_LINE)
            {
                pen.y -= m_char_max_height;
                pen.x = position.x;
            }
            else if (text_char == ASCII_SPACE)
            {
                // Advance
                pen.x += glyph.horizontal_advance;
            }
            else // Any other char
            {
                // First triangle in quad.        
                m_vertices.emplace_back(pen.x + glyph.offset_x,                 pen.y + glyph.offset_y,                  0.0f, glyph.uv_x_left,  glyph.uv_y_top);       // top left
                m_vertices.emplace_back(pen.x + glyph.offset_x  + glyph.width,  pen.y + glyph.offset_y - glyph.height,   0.0f, glyph.uv_x_right, glyph.uv_y_bottom);    // bottom right
                m_vertices.emplace_back(pen.x + glyph.offset_x,                 pen.y + glyph.offset_y - glyph.height,   0.0f, glyph.uv_x_left,  glyph.uv_y_bottom);    // bottom left
                // Second triangle in quad.
                m_vertices.emplace_back(pen.x + glyph.offset_x,                 pen.y + glyph.offset_y,                  0.0f, glyph.uv_x_left,  glyph.uv_y_top);       // top left
                m_vertices.emplace_back(pen.x + glyph.offset_x  + glyph.width,  pen.y + glyph.offset_y,                  0.0f, glyph.uv_x_right, glyph.uv_y_top);       // top right
                m_vertices.emplace_back(pen.x + glyph.offset_x  + glyph.width,  pen.y + glyph.offset_y - glyph.height,   0.0f, glyph.uv_x_right, glyph.uv_y_bottom);    // bottom right

                // Advance
                pen.x += glyph.horizontal_advance;
            }
        }
        m_vertices.shrink_to_fit();
        
        m_indices.clear();
        for (uint32_t i = 0; i < m_vertices.size(); i++)
        {
            m_indices.emplace_back(i);
        }

        UpdateBuffers(m_vertices, m_indices);
    }

    void Font::SetSize(const uint32_t size)
    {
        m_font_size = Helper::Clamp<uint32_t>(size, 8, 50);
    }

    void Font::UpdateBuffers(vector<RHI_Vertex_PosTex>& vertices, vector<uint32_t>& indices) const
    {
        SP_ASSERT(m_vertex_buffer != nullptr);
        SP_ASSERT(m_index_buffer != nullptr);

        // Grow buffers (if needed)
        if (vertices.size() > m_vertex_buffer->GetVertexCount())
        {
            m_vertex_buffer->CreateDynamic<RHI_Vertex_PosTex>(static_cast<uint32_t>(vertices.size()));
            m_index_buffer->CreateDynamic<uint32_t>(static_cast<uint32_t>(indices.size()));
        }

        if (const auto vertex_buffer = static_cast<RHI_Vertex_PosTex*>(m_vertex_buffer->GetMappedData()))
        {
            copy(vertices.begin(), vertices.end(), vertex_buffer);
        }

        if (const auto index_buffer = static_cast<uint32_t*>(m_index_buffer->GetMappedData()))
        {
            copy(indices.begin(), indices.end(), index_buffer);
        }
    }
}
