
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
#include "Runtime/Resource/asset_manager.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;
namespace LitchiRuntime
{

    Material::Material() {

    }

    Material::~Material() {

    }

    void Material::Parse(string material_path) {

        MaterialRes res;
        AssetManager::LoadAsset(material_path, res);

        DEBUG_LOG_INFO("Material::Parse :" + material_path);

        shader_ = Shader::Find(res.shader_path_);
        for (auto texture_res:res.textureRes_arr_)
        {
            textures_.push_back(std::make_pair(texture_res.texture_name_, Texture2D::LoadFromFile(texture_res.texture_path_)));
        }
    }

    void Material::SetUniformMatrix4fv(std::string shader_property_name, float* pointer) {
        uniform_matrix4fv_vec.push_back(std::make_pair(shader_property_name, pointer));
    }

    void Material::SetUniform1i(std::string shader_property_name, int value) {
        uniform_1i_vec.push_back(std::make_pair(shader_property_name, value));
    }
}



