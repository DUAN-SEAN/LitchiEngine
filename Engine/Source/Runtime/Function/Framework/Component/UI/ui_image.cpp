
#include "ui_image.h"
#include <vector>

#include "Runtime/Core/App/application_base.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

using namespace rttr;
using namespace LitchiRuntime;

UIImage::UIImage():Component(),texture2D_(nullptr),width_(0),height_(0) {

}

UIImage::~UIImage() {

}

void UIImage::Update() {
    Component::Update();
    if(texture2D_== nullptr){
        return;
    }
    MeshFilter* mesh_filter=game_object()->GetComponent<MeshFilter>();
    if(mesh_filter== nullptr){

        Vertex v00{ {0.f, 0.0f, 0.0f},{0.f, 0.f},{1.0f,1.0f,1.0f},{},{},{},{},{1.0f,1.0f,1.0f,1.0f} };
        Vertex v10{ {(float)texture2D_->width, 0.0f, 0.0f},{1.f, 0.f},{1.0f,1.0f,1.0f},{},{} ,{},{},{1.0f,1.0f,1.0f,1.0f} };
        Vertex v11{ {(float)texture2D_->width,  (float)texture2D_->height, 0.0f},{1.f, 1.f},{1.0f,1.0f,1.0f},{},{} ,{},{},{1.0f,1.0f,1.0f,1.0f} };
        Vertex v01{ {0.f,  (float)texture2D_->height, 0.0f},{0.f, 1.f},{1.0f,1.0f,1.0f},{},{} ,{},{},{1.0f,1.0f,1.0f,1.0f} };

        //创建 MeshFilter
        /*std::vector<Vertex> vertex_vector={
                { {0.f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.f, 0.f} },
                { {texture2D_->width(), 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.f, 0.f} },
                { {texture2D_->width(),  texture2D_->height(), 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.f, 1.f} },
                { {0.f,  texture2D_->height(), 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.f, 1.f} }
        };*/
    	std::vector<Vertex> vertex_vector={
            v00,v10,v11,v01
        };
        std::vector<unsigned int> index_vector={
                0,1,2,
                0,2,3
        };
        mesh_filter=game_object()->AddComponent<MeshFilter>();
        mesh_filter->CreateUIMesh(vertex_vector,index_vector);

        //挂上 MeshRenderer 组件
        auto mesh_renderer = game_object()->AddComponent<MeshRenderer>();
        mesh_renderer->material_path = "Engine\\Materials\\UIImage.mat";
        mesh_renderer->PostResourceLoaded();

        //创建 Material
        auto material = mesh_renderer->material();
        //使用贴图
        material->Set("u_DiffuseMap", texture2D_);

    }
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

