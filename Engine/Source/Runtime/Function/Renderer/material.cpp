
#include "material.h"
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "shader.h"
#include "texture2d.h"
#include "Runtime/Core/App/application.h"
#include "Runtime/Core/Log/debug.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;

Material::Material() {

}

Material::~Material() {

}

void Material::Parse(string material_path) {

    DEBUG_LOG_INFO("Parse");
    DEBUG_LOG_INFO(Application::GetDataPath());

    string dataPath = "D:/WorkSpace/LitchiEngineGit/LitchiEngineGithub/LitchiEngine/build/Engine/Source/Assets/";

    //解析xml
    rapidxml::file<> xml_file((dataPath +material_path).c_str());
    rapidxml::xml_document<> document;
    document.parse<0>(xml_file.data());

    //根节点
    rapidxml::xml_node<>* material_node=document.first_node("material");
    if(material_node == nullptr){
        return;
    }

    rapidxml::xml_attribute<>* material_shader_attribute=material_node->first_attribute("shader");
    if(material_shader_attribute == nullptr){
        return;
    }
    shader_=Shader::Find(material_shader_attribute->value());

    //解析Texture
    rapidxml::xml_node<>* material_texture_node=material_node->first_node("texture");
    while (material_texture_node != nullptr){
        rapidxml::xml_attribute<>* texture_name_attribute=material_texture_node->first_attribute("name");
        if(texture_name_attribute == nullptr){
            return;
        }

        rapidxml::xml_attribute<>* texture_image_attribute=material_texture_node->first_attribute("image");
        if(texture_image_attribute == nullptr){
            return;
        }

        std::string shader_property_name=texture_name_attribute->value();
        std::string image_path=texture_image_attribute->value();
        textures_.push_back(std::make_pair(texture_name_attribute->value(), Texture2D::LoadFromFile(image_path)));

        material_texture_node=material_texture_node->next_sibling("texture");
    }
}

void Material::SetUniformMatrix4fv(std::string shader_property_name, float *pointer) {
    uniform_matrix4fv_vec.push_back(std::make_pair(shader_property_name,pointer));
}

void Material::SetUniform1i(std::string shader_property_name, int value) {
    uniform_1i_vec.push_back(std::make_pair(shader_property_name,value));
}




