
#include "UIImage.h"

#include <vector>

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

using namespace rttr;
using namespace LitchiRuntime;

UIImage::UIImage():Component(),m_texture2D(nullptr),m_width(0),m_height(0) {

}

UIImage::~UIImage() {

}

void UIImage::Update() {
    // todo:
    //Component::Update();
    //if(m_texture2D== nullptr){
    //    return;
    //}
    //MeshFilter* mesh_filter=GetGameObject()->GetComponent<MeshFilter>();
    //if(mesh_filter== nullptr){

    //    Vertex v00{ {0.f, 0.0f, 0.0f},{0.f, 0.f},{1.0f,1.0f,1.0f},{},{},{},{},{1.0f,1.0f,1.0f,1.0f} };
    //    Vertex v10{ {(float)m_texture2D->width, 0.0f, 0.0f},{1.f, 0.f},{1.0f,1.0f,1.0f},{},{} ,{},{},{1.0f,1.0f,1.0f,1.0f} };
    //    Vertex v11{ {(float)m_texture2D->width,  (float)m_texture2D->height, 0.0f},{1.f, 1.f},{1.0f,1.0f,1.0f},{},{} ,{},{},{1.0f,1.0f,1.0f,1.0f} };
    //    Vertex v01{ {0.f,  (float)m_texture2D->height, 0.0f},{0.f, 1.f},{1.0f,1.0f,1.0f},{},{} ,{},{},{1.0f,1.0f,1.0f,1.0f} };

    //    //创建 MeshFilter
    //    /*std::vector<Vertex> vertex_vector={
    //            { {0.f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.f, 0.f} },
    //            { {texture2D_->width(), 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.f, 0.f} },
    //            { {texture2D_->width(),  texture2D_->height(), 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.f, 1.f} },
    //            { {0.f,  texture2D_->height(), 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.f, 1.f} }
    //    };*/
    //	std::vector<Vertex> vertex_vector={
    //        v00,v10,v11,v01
    //    };
    //    std::vector<unsigned int> index_vector={
    //            0,1,2,
    //            0,2,3
    //    };
    //    mesh_filter=GetGameObject()->AddComponent<MeshFilter>();
    //    mesh_filter->CreateUIMesh(vertex_vector,index_vector);

    //    //挂上 MeshRenderer 组件
    //    auto mesh_renderer = GetGameObject()->AddComponent<MeshRenderer>();
    //    mesh_renderer->materialPath = "Engine\\Materials\\UIImage.mat";
    //    mesh_renderer->PostResourceLoaded();

    //    //创建 Material
    //    auto material = mesh_renderer->GetMaterial();
    //    //使用贴图
    //    material->Set("u_DiffuseMap", m_texture2D);

    //}
}

void UIImage::OnPreRender() {
    Component::OnPreRender();
    //glStencilFunc(GL_EQUAL, 0x1, 0xFF);//等于1 通过测试 ,就是上次绘制的图 的范围 才通过测试。
    //glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);//没有通过测试的，保留原来的，也就是保留上一次的值。
}

void UIImage::OnPostRender() {
    Component::OnPostRender();
    //RenderDevice::instance()->Disable(RenderDevice::STENCIL_TEST);
}

