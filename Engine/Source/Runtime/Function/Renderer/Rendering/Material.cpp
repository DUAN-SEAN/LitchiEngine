
//= INCLUDES =========================
#include "Runtime/Core/pch.h"
#include "Material.h"
#include "Renderer.h"
#include "../Resource/ResourceCache.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_TextureCube.h"
//====================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
    Material::Material() : IResource(ResourceType::Material)
    {
    }

    bool Material::LoadFromFile(const string& file_path)
    {
        {
        // todo:
        //auto xml = make_unique<XmlDocument>();
        //if (!xml->Load(file_path))
        //    return false;

        //SetResourceFilePath(file_path);

        //xml->GetAttribute("Material", "color_r",                         &m_properties[static_cast<uint32_t>(MaterialProperty::ColorR)]);
        //xml->GetAttribute("Material", "color_g",                         &m_properties[static_cast<uint32_t>(MaterialProperty::ColorG)]);
        //xml->GetAttribute("Material", "color_b",                         &m_properties[static_cast<uint32_t>(MaterialProperty::ColorB)]);
        //xml->GetAttribute("Material", "color_a",                         &m_properties[static_cast<uint32_t>(MaterialProperty::ColorA)]);
        //xml->GetAttribute("Material", "roughness_multiplier",            &m_properties[static_cast<uint32_t>(MaterialProperty::RoughnessMultiplier)]);
        //xml->GetAttribute("Material", "metalness_multiplier",            &m_properties[static_cast<uint32_t>(MaterialProperty::MetalnessMultiplier)]);
        //xml->GetAttribute("Material", "normal_multiplier",               &m_properties[static_cast<uint32_t>(MaterialProperty::NormalMultiplier)]);
        //xml->GetAttribute("Material", "height_multiplier",               &m_properties[static_cast<uint32_t>(MaterialProperty::HeightMultiplier)]);
        //xml->GetAttribute("Material", "clearcoat_multiplier",            &m_properties[static_cast<uint32_t>(MaterialProperty::Clearcoat)]);
        //xml->GetAttribute("Material", "clearcoat_roughness_multiplier",  &m_properties[static_cast<uint32_t>(MaterialProperty::Clearcoat_Roughness)]);
        //xml->GetAttribute("Material", "anisotropic_multiplier",          &m_properties[static_cast<uint32_t>(MaterialProperty::Anisotropic)]);
        //xml->GetAttribute("Material", "anisotropic_rotation_multiplier", &m_properties[static_cast<uint32_t>(MaterialProperty::AnisotropicRotation)]);
        //xml->GetAttribute("Material", "sheen_multiplier",                &m_properties[static_cast<uint32_t>(MaterialProperty::Sheen)]);
        //xml->GetAttribute("Material", "sheen_tint_multiplier",           &m_properties[static_cast<uint32_t>(MaterialProperty::SheenTint)]);
        //xml->GetAttribute("Material", "uv_tiling_x",                     &m_properties[static_cast<uint32_t>(MaterialProperty::UvTilingX)]);
        //xml->GetAttribute("Material", "uv_tiling_y",                     &m_properties[static_cast<uint32_t>(MaterialProperty::UvTilingY)]);
        //xml->GetAttribute("Material", "uv_offset_x",                     &m_properties[static_cast<uint32_t>(MaterialProperty::UvOffsetX)]);
        //xml->GetAttribute("Material", "uv_offset_y",                     &m_properties[static_cast<uint32_t>(MaterialProperty::UvOffsetY)]);
        //xml->GetAttribute("Material", "can_be_edited",                   &m_properties[static_cast<uint32_t>(MaterialProperty::CanBeEdited)]);

        //const uint32_t texture_count = xml->GetAttributeAs<uint32_t>("textures", "count");
        //for (uint32_t i = 0; i < texture_count; i++)
        //{
        //    auto node_name                 = "texture_" + to_string(i);
        //    const MaterialTexture tex_type = static_cast<MaterialTexture>(xml->GetAttributeAs<uint32_t>(node_name, "texture_type"));
        //    auto tex_name                  = xml->GetAttributeAs<string>(node_name, "texture_name");
        //    auto tex_path                  = xml->GetAttributeAs<string>(node_name, "texture_path");

        //    // If the texture happens to be loaded, get a reference to it
        //    auto texture = ResourceCache::GetByName<RHI_Texture2D>(tex_name);
        //    // If there is not texture (it's not loaded yet), load it
        //    if (!texture)
        //    {
        //        texture = ResourceCache::Load<RHI_Texture2D>(tex_path);
        //    }

        //    SetTexture(tex_type, texture);
        //}

        //m_object_size_cpu = sizeof(*this);
	        
        }

        // load material json

        // load shader

        // load material texture path
        // load 

        // get descriptor and set data


        return true;
    }

    bool Material::SaveToFile(const string& file_path)
    {
        SetResourceFilePath(file_path);

        // todo:
       /* auto xml = make_unique<XmlDocument>();
        xml->AddNode("Material");
        xml->AddAttribute("Material", "color_r",                         GetProperty(MaterialProperty::ColorR));
        xml->AddAttribute("Material", "color_g",                         GetProperty(MaterialProperty::ColorG));
        xml->AddAttribute("Material", "color_b",                         GetProperty(MaterialProperty::ColorB));
        xml->AddAttribute("Material", "color_a",                         GetProperty(MaterialProperty::ColorA));
        xml->AddAttribute("Material", "roughness_multiplier",            GetProperty(MaterialProperty::RoughnessMultiplier));
        xml->AddAttribute("Material", "metalness_multiplier",            GetProperty(MaterialProperty::MetalnessMultiplier));
        xml->AddAttribute("Material", "normal_multiplier",               GetProperty(MaterialProperty::NormalMultiplier));
        xml->AddAttribute("Material", "height_multiplier",               GetProperty(MaterialProperty::HeightMultiplier));
        xml->AddAttribute("Material", "clearcoat_multiplier",            GetProperty(MaterialProperty::Clearcoat));
        xml->AddAttribute("Material", "clearcoat_roughness_multiplier",  GetProperty(MaterialProperty::Clearcoat_Roughness));
        xml->AddAttribute("Material", "anisotropic_multiplier",          GetProperty(MaterialProperty::Anisotropic));
        xml->AddAttribute("Material", "anisotropic_rotation_multiplier", GetProperty(MaterialProperty::AnisotropicRotation));
        xml->AddAttribute("Material", "sheen_multiplier",                GetProperty(MaterialProperty::Sheen));
        xml->AddAttribute("Material", "sheen_tint_multiplier",           GetProperty(MaterialProperty::SheenTint));
        xml->AddAttribute("Material", "uv_tiling_x",                     GetProperty(MaterialProperty::UvTilingX));
        xml->AddAttribute("Material", "uv_tiling_y",                     GetProperty(MaterialProperty::UvTilingY));
        xml->AddAttribute("Material", "uv_offset_x",                     GetProperty(MaterialProperty::UvOffsetX));
        xml->AddAttribute("Material", "uv_offset_y",                     GetProperty(MaterialProperty::UvOffsetY));
        xml->AddAttribute("Material", "can_be_edited",                   GetProperty(MaterialProperty::CanBeEdited));

        xml->AddChildNode("Material", "textures");
        xml->AddAttribute("textures", "count", static_cast<uint32_t>(m_textures.size()));
        uint32_t i = 0;
        for (const auto& texture : m_textures)
        {
            auto tex_node = "texture_" + to_string(i);
            xml->AddChildNode("textures", tex_node);
            xml->AddAttribute(tex_node, "texture_type", i++);
            xml->AddAttribute(tex_node, "texture_name", texture ? texture->GetObjectName() : "");
            xml->AddAttribute(tex_node, "texture_path", texture ? texture->GetResourceFilePathNative() : "");
        }

        return xml->Save(GetResourceFilePathNative());*/
        return  false;
    }

    void Material::SetTexture(const int slot, RHI_Texture* texture)
    {
    }

    void Material::SetValue(const int slot, const std::vector<std::string>& pathArr, const UniformType uniformType, const std::any value)
    {
    }

    //void Material::SetTexture(const MaterialTexture texture_type, RHI_Texture* texture)
    //{
    //    uint32_t type_int = static_cast<uint32_t>(texture_type);

    //    if (texture)
    //    {
    //        // Cache the texture to ensure scene serialization/deserialization
    //        m_textures[type_int] = ResourceCache::Cache(texture->GetSharedPtr());
    //    }
    //    else
    //    {
    //        m_textures[type_int] = nullptr;
    //    }

    //    // Set the correct multiplier
    //    float multiplier = texture != nullptr;
    //    if (texture_type == MaterialTexture::Roughness)
    //    {
    //        SetProperty(MaterialProperty::RoughnessMultiplier, multiplier);
    //    }
    //    else if (texture_type == MaterialTexture::Metalness)
    //    {
    //        SetProperty(MaterialProperty::MetalnessMultiplier, multiplier);
    //    }
    //    else if (texture_type == MaterialTexture::Normal)
    //    {
    //        SetProperty(MaterialProperty::NormalMultiplier, multiplier);
    //    }
    //    else if (texture_type == MaterialTexture::Height)
    //    {
    //        SetProperty(MaterialProperty::HeightMultiplier, multiplier);
    //    }
    //}

    //void Material::SetTexture(const MaterialTexture texture_type, shared_ptr<RHI_Texture> texture)
    //{
    //    SetTexture(texture_type, texture.get());
    //}

    //void Material::SetTexture(const MaterialTexture type, shared_ptr<RHI_Texture2D> texture)
    //{
    //    SetTexture(type, static_pointer_cast<RHI_Texture>(texture));
    //}

    //void Material::SetTexture(const MaterialTexture type, shared_ptr<RHI_TextureCube> texture)
    //{
    //    SetTexture(type, static_pointer_cast<RHI_Texture>(texture));
    //}

    //bool Material::HasTexture(const string& path) const
    //{
    //    for (const auto& texture : m_textures)
    //    {
    //        if (!texture)
    //            continue;

    //        if (texture->GetResourceFilePathNative() == path)
    //            return true;
    //    }

    //    return false;
    //}

    //bool Material::HasTexture(const MaterialTexture texture_type) const
    //{
    //    return m_textures[static_cast<uint32_t>(texture_type)] != nullptr;
    //}

    //string Material::GetTexturePathByType(const MaterialTexture texture_type)
    //{
    //    if (!HasTexture(texture_type))
    //        return "";

    //    return m_textures[static_cast<uint32_t>(texture_type)]->GetResourceFilePathNative();
    //}

    //vector<string> Material::GetTexturePaths()
    //{
    //    vector<string> paths;
    //    for (const auto& texture : m_textures)
    //    {
    //        if (!texture)
    //            continue;

    //        paths.emplace_back(texture->GetResourceFilePathNative());
    //    }

    //    return paths;
    //}

    //RHI_Texture* Material::GetTexture(const MaterialTexture texture_type)
    //{
    //    return GetTexture_PtrShared(texture_type).get();
    //}

    //shared_ptr<RHI_Texture>& Material::GetTexture_PtrShared(const MaterialTexture texture_type)
    //{
    //    static shared_ptr<RHI_Texture> texture_empty;
    //    return HasTexture(texture_type) ? m_textures[static_cast<uint32_t>(texture_type)] : texture_empty;
    //}

    //void Material::SetProperty(const MaterialProperty property_type, const float value)
    //{
    //    if (m_properties[static_cast<uint32_t>(property_type)] == value)
    //        return;

    //    if (property_type == MaterialProperty::ColorA)
    //    {
    //        // If an object switches from opaque to transparent or vice versa, make the world update so that the renderer
    //        // goes through the entities and makes the ones that use this material, render in the correct mode.
    //        float current_alpha = m_properties[static_cast<uint32_t>(property_type)];
    //        if ((current_alpha != 1.0f && value == 1.0f) || (current_alpha == 1.0f && value != 1.0f))
    //        {
    //            // todo:
    //            // World::Resolve();
    //        }

    //        // Transparent objects are typically see-through (low roughness) so use the alpha as the roughness multiplier.
    //        m_properties[static_cast<uint32_t>(MaterialProperty::RoughnessMultiplier)] = value * 0.5f;
    //    }

    //    m_properties[static_cast<uint32_t>(property_type)] = value;
    //}

    //void Material::SetColor(const Color& color)
    //{
    //    SetProperty(MaterialProperty::ColorR, color.r);
    //    SetProperty(MaterialProperty::ColorG, color.g);
    //    SetProperty(MaterialProperty::ColorB, color.b);
    //    SetProperty(MaterialProperty::ColorA, color.a);
    //}
}
