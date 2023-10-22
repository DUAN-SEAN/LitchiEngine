
#include "UIText.h"

#include <vector>
#include <rttr/registration>

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Function/Renderer/Font/font.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

using namespace rttr;
using namespace LitchiRuntime;

UIText::UIText():m_color({1,1,1,1}) {

}

void UIText::SetText(std::string text) {
    if(m_text==text){
        return;
    }
    m_text=text;
    m_dirty=true;
}

void UIText::Update() {
    Component::Update();

    // todo
    //if(m_font== nullptr || m_font->font_texture()== nullptr){
    //    return;
    //}

    //MeshFilter* mesh_filter=GetGameObject()->GetComponent<MeshFilter>();
    //if(mesh_filter== nullptr){
    //    //挂上 MeshFilter 组件
    //    mesh_filter=GetGameObject()->AddComponent<MeshFilter>();
    //    
    //    //挂上 MeshRenderer 组件
    //    auto mesh_renderer=GetGameObject()->AddComponent<MeshRenderer>();
    //    mesh_renderer->materialPath = "Engine\\Materials\\UIText.mat";
    //    mesh_renderer->PostResourceLoaded();

    //    //使用文字贴图
    //    mesh_renderer->GetMaterial()->Set("u_DiffuseMap", m_font->font_texture());
    //}

    //if(m_dirty){
    //    m_dirty=false;

    //    std::vector<Font::Character*> character_vec=m_font->LoadStr(m_text);
    //    //遍历每个字符进行绘制
    //    std::vector<Vertex> vertex_vector;
    //    std::vector<unsigned int> index_vector(character_vec.size()*6);

    //    int x=0;
    //    std::vector<unsigned short> index={0, 1, 2, 0, 2, 3};
    //    
    //    for (int i = 0; i < character_vec.size(); ++i) {
    //        auto character=character_vec[i];
    //        unsigned short width=(character->right_bottom_x_-character->left_top_x_)* m_font->font_texture()->width;
    //        unsigned short height=(character->right_bottom_y_-character->left_top_y_)* m_font->font_texture()->height;
    //        //因为FreeType生成的bitmap是上下颠倒的，所以这里UV坐标也要做对应翻转，将左上角作为零点。

    //        Vertex v00{ {(float)x,0.0f,0.0f},{character->left_top_x_,     character->right_bottom_y_},{},{},{},{},{},{m_color.x,m_color.y,m_color.z,m_color.w} };
    //        Vertex v10{ {(float)(x + width),0.0f,0.0f},{character->right_bottom_x_, character->right_bottom_y_},{},{},{},{},{},{m_color.x,m_color.y,m_color.z,m_color.w} };
    //        Vertex v11{ {(float)(x + width),(float)height,0.0f},{character->right_bottom_x_, character->left_top_y_},{},{},{},{},{},{m_color.x,m_color.y,m_color.z,m_color.w} };
    //        Vertex v01{ {(float)x,(float)height,0.0f},{character->left_top_x_,     character->left_top_y_},{},{},{},{},{},{m_color.x,m_color.y,m_color.z,m_color.w} };

    //        vertex_vector.insert(vertex_vector.end() ,{ v00,v10,v11,v01 });

    //       /* vertex_vector.insert(vertex_vector.end(),{
    //                {{x,0.0f, 0.0f}, m_color, {character->left_top_x_,     character->right_bottom_y_}},
    //                {{x+width,0.0f, 0.0f}, m_color, {character->right_bottom_x_, character->right_bottom_y_}},
    //                {{x+width,height, 0.0f}, m_color, {character->right_bottom_x_, character->left_top_y_}},
    //                {{x,height, 0.0f}, m_color, {character->left_top_x_,     character->left_top_y_}}
    //        });*/
    //        x+=width;


    //        for (int j = 0; j < index.size(); ++j) {
    //            index_vector[i*index.size()+j]=index[j]+i*4;
    //        }
    //    }
    //    
    //    mesh_filter->CreateUIMesh(vertex_vector,index_vector);
    //}
}

void UIText::OnPreRender() {
    Component::OnPreRender();
}

void UIText::OnPostRender() {
    Component::OnPostRender();
}

UIText::~UIText() {

}
