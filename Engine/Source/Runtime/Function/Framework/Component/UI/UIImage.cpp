
#include "Runtime/Core/pch.h"
#include "UIImage.h"

#include <vector>

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RHI/RHI_IndexBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_VertexBuffer.h"

using namespace rttr;
using namespace LitchiRuntime;

UIImage::UIImage():Component(),m_texture2D(nullptr), m_color({ 1,1,1,1 }) {

}

UIImage::~UIImage() {

    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
}

void UIImage::OnAwake()
{
}

void UIImage::OnUpdate() {
    
}

void UIImage::PostResourceLoaded()
{
    CreateBuffer();
    PostResourceModify();
}

void UIImage::PostResourceModify()
{
    if(!m_imagePath.empty())
    {
        m_texture2D = ApplicationBase::Instance()->textureManager->LoadResource(m_imagePath);
        if(m_texture2D == nullptr)
        {
            m_imagePath = "";
        }
    }
	else
    {
        m_texture2D = nullptr;
    }
}

void UIImage::CreateBuffer()
{
    if (m_vertexBuffer != nullptr)
    {
        return;
    }
    // create buffer
    m_vertexBuffer = std::make_shared<RHI_VertexBuffer>(true, "image");
    m_indexBuffer = std::make_shared<RHI_IndexBuffer>(true, "image");

    RHI_Vertex_PosTex left_top{ {0.f, 1.0f, 0.0f},{0.f, 0.f} };
    RHI_Vertex_PosTex right_bottom{ {1.f,  0.0f, 0.0f},{1.f, 1.f} };
    RHI_Vertex_PosTex left_bottom{ {0.0f,  0.0f, 0.0f},{0.f, 1.f} };
    RHI_Vertex_PosTex right_top{ {1.0, 1.0f, 0.0f},{1.f, 0.f} };
    std::vector<RHI_Vertex_PosTex> vertex_vector;
    vertex_vector.emplace_back(left_top);
    vertex_vector.emplace_back(right_bottom);
    vertex_vector.emplace_back(left_bottom);
    vertex_vector.emplace_back(right_top);
    std::vector<unsigned int> index_vector = { 0,1,2,0,3,1 };

    //创建 Mesh
    m_vertexBuffer->CreateDynamic<RHI_Vertex_PosTex>(static_cast<uint32_t>(vertex_vector.size()));
    m_indexBuffer->CreateDynamic<uint32_t>(static_cast<uint32_t>(index_vector.size()));

    // copy the data over to the gpu
    {
        if (RHI_Vertex_PosTex* vertex_buffer = static_cast<RHI_Vertex_PosTex*>(m_vertexBuffer->GetMappedData()))
        {
            std::copy(vertex_vector.begin(), vertex_vector.end(), vertex_buffer);
        }

        if (uint32_t* index_buffer = static_cast<uint32_t*>(m_indexBuffer->GetMappedData()))
        {
            std::copy(index_vector.begin(), index_vector.end(), index_buffer);
        }
    }
}

