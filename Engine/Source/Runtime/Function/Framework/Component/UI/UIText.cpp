
#include "Runtime/Core/pch.h"
#include "UIText.h"

#include <memory>
#include <vector>
#include <rttr/registration>

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RHI/RHI_IndexBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_VertexBuffer.h"

using namespace rttr;
using namespace LitchiRuntime;

UIText::UIText() :m_color({ 1,1,1,1 }), m_dirty{ true } {
}

void UIText::SetText(std::string text) {
    if(m_text==text){
        return;
    }
    m_text=text;
    m_dirty=true;
}

void UIText::OnAwake()
{
}

void UIText::OnUpdate() {
    Component::OnUpdate();

    UpdateTextContent();
}

void UIText::OnEditorUpdate()
{
    UpdateTextContent();
}

void UIText::OnPreRender() {
    Component::OnPreRender();
}

void UIText::OnPostRender() {
    Component::OnPostRender();
}

void UIText::PostResourceLoaded()
{
    CreateTextBuffer();
    // default
    PostResourceModify();
    UpdateTextContent();
}

void UIText::PostResourceModify()
{
    if(!m_font_path.empty())
    {
        m_font = ApplicationBase::Instance()->fontManager->LoadResource(m_font_path);
    }
}

void UIText::CreateTextBuffer()
{
    if(!m_vertex_buffer)
    {
        // create buffer
        m_vertex_buffer = std::make_shared<RHI_VertexBuffer>(true, "font");
        m_index_buffer = std::make_shared<RHI_IndexBuffer>(true, "font");
    }
}

void UIText::UpdateTextContent()
{
    if (m_font == nullptr)
    {
        return;
    }

    if (m_dirty)
    {
        TextData text_data = m_font->GetTextData(m_text);

        // create/grow buffers
        if (text_data.vertices.size() != m_vertex_buffer->GetVertexCount())
        {
            m_vertex_buffer->CreateDynamic<RHI_Vertex_PosTex>(static_cast<uint32_t>(text_data.vertices.size()));
            m_index_buffer->CreateDynamic<uint32_t>(static_cast<uint32_t>(text_data.indices.size()));
        }

        // copy the data over to the gpu
        {
            if (RHI_Vertex_PosTex* vertex_buffer = static_cast<RHI_Vertex_PosTex*>(m_vertex_buffer->GetMappedData()))
            {
                std::copy(text_data.vertices.begin(), text_data.vertices.end(), vertex_buffer);
            }

            if (uint32_t* index_buffer = static_cast<uint32_t*>(m_index_buffer->GetMappedData()))
            {
                std::copy(text_data.indices.begin(), text_data.indices.end(), index_buffer);
            }
        }

    }
}

UIText::~UIText() {

}
