
#include "ui_text.h"
#include <vector>
#include <rttr/registration>

#include "Runtime/Core/App/application_base.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Function/Renderer/Font/font.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

using namespace rttr;
using namespace LitchiRuntime;

UIText::UIText():m_color({1,1,1,1}) {

}

void UIText::set_text(std::string text) {
    if(m_text==text){
        return;
    }
    m_text=text;
    m_dirty=true;
}

void UIText::Update() {
    Component::Update();

    if(m_font== nullptr || m_font->font_texture()== nullptr){
        return;
    }

    MeshFilter* mesh_filter=game_object()->GetComponent<MeshFilter>();
    if(mesh_filter== nullptr){
        //挂上 MeshFilter 组件
        mesh_filter=game_object()->AddComponent<MeshFilter>();

        //创建 Material
        auto material= ApplicationBase::Instance()->materialManager->GetResource("Engine\\Materials\\UIText.mat");
           
        //挂上 MeshRenderer 组件
        auto mesh_renderer=game_object()->AddComponent<MeshRenderer>();
        mesh_renderer->SetMaterial(material);

        //使用文字贴图
        material->Set("u_diffuse_texture", m_font->font_texture());
    }

    if(m_dirty){
        m_dirty=false;

        std::vector<Font::Character*> character_vec=m_font->LoadStr(m_text);
        //遍历每个字符进行绘制
        std::vector<Vertex> vertex_vector;
        std::vector<unsigned int> index_vector(character_vec.size()*6);

        int x=0;
        std::vector<unsigned short> index={0, 1, 2, 0, 2, 3};
        
        for (int i = 0; i < character_vec.size(); ++i) {
            auto character=character_vec[i];
            unsigned short width=(character->right_bottom_x_-character->left_top_x_)* m_font->font_texture()->width;
            unsigned short height=(character->right_bottom_y_-character->left_top_y_)* m_font->font_texture()->height;
            //因为FreeType生成的bitmap是上下颠倒的，所以这里UV坐标也要做对应翻转，将左上角作为零点。

            Vertex v00{ {(float)x,0.0f,0.0f},{character->left_top_x_,     character->right_bottom_y_},{},{},{}};
            Vertex v10{ {(float)(x + width,0.0f, 0.0f),0.0f,0.0f},{character->right_bottom_x_, character->right_bottom_y_},{},{},{}};
            Vertex v11{ {(float)(x + width),(float)height,0.0f},{character->right_bottom_x_, character->left_top_y_},{},{},{}};
            Vertex v01{ {(float)x,(float)height,0.0f},{character->left_top_x_,     character->left_top_y_},{},{},{}};

            vertex_vector.insert(vertex_vector.end() ,{ v00,v10,v11,v01 });

           /* vertex_vector.insert(vertex_vector.end(),{
                    {{x,0.0f, 0.0f}, m_color, {character->left_top_x_,     character->right_bottom_y_}},
                    {{x+width,0.0f, 0.0f}, m_color, {character->right_bottom_x_, character->right_bottom_y_}},
                    {{x+width,height, 0.0f}, m_color, {character->right_bottom_x_, character->left_top_y_}},
                    {{x,height, 0.0f}, m_color, {character->left_top_x_,     character->left_top_y_}}
            });*/
            x+=width;


            for (int j = 0; j < index.size(); ++j) {
                index_vector[i*index.size()+j]=index[j]+i*4;
            }
        }
        
        mesh_filter->CreateMesh(vertex_vector,index_vector);
    }
}

void UIText::OnPreRender() {
    Component::OnPreRender();
}

void UIText::OnPostRender() {
    Component::OnPostRender();
}

UIText::~UIText() {

}
