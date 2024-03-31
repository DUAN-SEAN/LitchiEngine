
//= INCLUDES =========================
#include "Runtime/Core/pch.h"
#include "Material.h"
#include "Renderer.h"
#include "../Resource/ResourceCache.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_TextureCube.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/RHI/RHI_ConstantBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_Shader.h"
#include "Runtime/Resource/AssetManager.h"
#include "Runtime/Resource/ShaderManager.h"
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
		// load material json
		MaterialRes* materialRes = new MaterialRes();
		if (!AssetManager::LoadAsset(file_path, *materialRes))
		{
			return false;
		}

		LC_ASSERT_MSG(materialRes->vertexType != RHI_Vertex_Type::Undefined, "Load Material Not Specify VertexType");

		SetResourceFilePath(file_path);
		m_materialRes = materialRes;

		// load shader
		if (m_materialRes->shaderPath.empty())
		{
			return true;
		}

		m_shader = ApplicationBase::Instance()->shaderManager->LoadResource(m_materialRes->shaderPath);

		PostResourceLoaded();

		return true;
	}

	bool Material::SaveToFile(const string& file_path)
	{
		SetResourceFilePath(file_path);

		if (m_shader)
		{
			// save data to materialRes
			m_materialRes->shaderPath = m_shader->m_shaderPath;

			// 清理已存在的uniformData
			for (auto value : m_materialRes->uniformInfoList)
			{
				delete value;
			}
			m_materialRes->uniformInfoList.clear();

			for (auto& [name, value] : m_uniformDataList)
			{
				auto shaderUniform = m_shader->GetGlobalUniformInfo(name);

				if (shaderUniform.type != UniformType::UNIFORM_Unknown)
				{
					switch (shaderUniform.type)
					{
					case UniformType::UNIFORM_BOOL:
						if (value.type() == typeid(bool))
						{
							auto uniformBool = new UniformInfoBool();
							uniformBool->name = shaderUniform.name;
							uniformBool->value = std::any_cast<bool>(value);
							m_materialRes->uniformInfoList.push_back(uniformBool);
						}
						break;
					case UniformType::UNIFORM_INT:
						if (value.type() == typeid(int))
						{
							auto uniformInt = new UniformInfoInt();
							uniformInt->name = shaderUniform.name;
							uniformInt->value = std::any_cast<int>(value);
							m_materialRes->uniformInfoList.push_back(uniformInt);

						}
						break;
					case UniformType::UNIFORM_FLOAT:
						if (value.type() == typeid(float))
						{
							auto uniformFloat = new UniformInfoFloat();
							uniformFloat->name = shaderUniform.name;
							uniformFloat->value = std::any_cast<float>(value);
							m_materialRes->uniformInfoList.push_back(uniformFloat);
						}
						break;
					case UniformType::UNIFORM_FLOAT_VEC2:
						if (value.type() == typeid(Vector2))
						{
							auto uniformVec2 = new UniformInfoVector2();
							uniformVec2->name = shaderUniform.name;
							uniformVec2->vector = std::any_cast<Vector2>(value);
							m_materialRes->uniformInfoList.push_back(uniformVec2);
						}
						break;
					case UniformType::UNIFORM_FLOAT_VEC3:
						if (value.type() == typeid(Vector3))
						{
							auto uniformVec3 = new UniformInfoVector3();
							uniformVec3->name = shaderUniform.name;
							uniformVec3->vector = std::any_cast<Vector3>(value);
							m_materialRes->uniformInfoList.push_back(uniformVec3);
						}
						break;
					case UniformType::UNIFORM_FLOAT_VEC4:
						if (value.type() == typeid(Vector4))
						{
							auto uniformVec4 = new UniformInfoVector4();
							uniformVec4->name = shaderUniform.name;
							uniformVec4->vector = std::any_cast<Vector4>(value);
							m_materialRes->uniformInfoList.push_back(uniformVec4);
						}
						break;
						//case UniformType::UNIFORM_TEXTURE:
						//	// if (value.type() == typeid(RHI_Texture*))
						//{
						//	auto uniformPath = new UniformInfoTexture();
						//	uniformPath->name = shaderUniform.name;
						//	uniformPath->path = "";
						//	if (auto tex = std::any_cast<RHI_Texture*>(value); tex)
						//	{
						//		uniformPath->path = tex->GetResourceFilePath();
						//	}
						//	m_materialRes->uniformInfoList.push_back(uniformPath);
						//}
						//break;
					default:
						DEBUG_LOG_ERROR("RebuildResourceFromMemory Not Found UniformType type:{}", shaderUniform.type);
						break;
					}
				}
				else
				{
					auto& descriptor = m_shader->GetTextureDescriptor(name);
					if (descriptor.type == RHI_Descriptor_Type::Texture)
					{
						auto uniformPath = new UniformInfoTexture();
						uniformPath->name = descriptor.name;
						uniformPath->path = "";
						if (value.type() == typeid(RHI_Texture*))
						{
							if (auto tex = std::any_cast<RHI_Texture*>(value); tex)
							{
								uniformPath->path = tex->GetResourceFilePathAsset();
							}
						}

						m_materialRes->uniformInfoList.push_back(uniformPath);
					}
					else
					{
						DEBUG_LOG_ERROR("Uniform Not Found GlobalValue or Texture in Shader name:{}", name);
					}
				}
			}



		}

		if (!AssetManager::SaveAsset(*m_materialRes, file_path))
		{
			DEBUG_LOG_ERROR("Save Fail path:{}", file_path);
			return false;
		}

		return  true;
	}

	void Material::SetTexture(const std::string& name, RHI_Texture* texture)
	{
		if (m_uniformDataList.find(name) != m_uniformDataList.end())
		{
			m_uniformDataList[name] = std::make_any<RHI_Texture*>(texture);
		}
	}

	void Material::SetShader(MaterialShader* shader)
	{
		if (!shader)
		{
			return;
		}

		if (m_shader)
		{
			delete m_shader;
		}

		m_shader = shader;

		// todo: update value and texture
		// m_valueMap
		// m_textureMap
	}

	void* Material::GetValues4DescriptorSet(uint32_t& size)
	{
		if(m_isValueDirty)
		{
			// todo: 每帧只用更新一次
			EASY_FUNCTION(profiler::colors::Brown200)
			for (auto& uniform : m_uniformDataList)
			{
				UpdateValue(uniform.first);
			}
			m_isValueDirty = false;
		}
		size = m_valueSize;

		return m_value;
	}

	std::map<int, RHI_Texture*> Material::GetTextures4DescriptorSet()
	{
		std::map<int, RHI_Texture*> textureMap;
		for (auto& uniform : m_uniformDataList)
		{
			auto name = uniform.first;
			auto& descriptor = m_shader->GetTextureDescriptor(name);
			if (descriptor.type == RHI_Descriptor_Type::Texture)
			{
				auto slot = descriptor.slot;
				if(uniform.second.type() == typeid(RHI_Texture*))
				{
					textureMap[slot] = any_cast<RHI_Texture*>(uniform.second);
				}
			}
		}

		return textureMap;
	}

	void Material::PostResourceModify()
	{
		// check shader is change
		// if change rebuild material
	}

	void Material::PostResourceLoaded()
	{
		if (!m_materialRes)
		{
			return;
		}

		if (!m_shader)
		{
			return;
		}

		// fill uniformDatas by shader
		m_uniformDataList.clear();
		auto uniformInfoList = m_materialRes->uniformInfoList;
		auto& shaderUniformDict = m_shader->GetGlobalShaderUniformDict();
		auto textureDescriptorDict = m_shader->GetTextureDescriptorDict();
		for (const auto& element : shaderUniformDict)
		{
			m_uniformDataList.emplace(element.first, std::any());
		}
		for (const auto& element : textureDescriptorDict)
		{
			m_uniformDataList.emplace(element.first, std::any());
		}

		// load value and texuture from material
		for (auto uniformInfo : uniformInfoList)
		{
			auto& uniformData = m_uniformDataList.find(uniformInfo->name);
			if (uniformData == m_uniformDataList.end())
			{
				DEBUG_LOG_ERROR("Material::PostResourceLoaded Not Found Uniform uniformName:{}", uniformInfo->name);
				continue;

			}
			switch (uniformInfo->GetUniformType())
			{
			case UniformType::UNIFORM_BOOL:
			{
				auto uniformInfoBool = static_cast<UniformInfoBool*>(uniformInfo);
				uniformData->second = std::make_any<bool>(uniformInfoBool->value);
				break;
			}
			case UniformType::UNIFORM_FLOAT:
			{
				auto uniformInfoFloat = static_cast<UniformInfoFloat*>(uniformInfo);
				uniformData->second = std::make_any<float>(uniformInfoFloat->value);
				break;
			}
			case UniformType::UNIFORM_INT:
			{
				auto uniformInfoInt = static_cast<UniformInfoInt*>(uniformInfo);
				uniformData->second = std::make_any<int>(uniformInfoInt->value);
				break;
			}
			case UniformType::UNIFORM_FLOAT_VEC2:
			{
				auto uniformInfoVector2 = static_cast<UniformInfoVector2*>(uniformInfo);
				uniformData->second = std::make_any<Vector2>(uniformInfoVector2->vector);
				break;
			}
			case UniformType::UNIFORM_FLOAT_VEC3:
			{
				auto uniformInfoVector3 = static_cast<UniformInfoVector3*>(uniformInfo);
				uniformData->second = std::make_any<Vector3>(uniformInfoVector3->vector);
				break;
			}
			case UniformType::UNIFORM_FLOAT_VEC4:
			{
				auto uniformInfoVector4 = static_cast<UniformInfoVector4*>(uniformInfo);
				uniformData->second = std::make_any<Vector4>(uniformInfoVector4->vector);
				break;
			}
			// todo:
			case UniformType::UNIFORM_TEXTURE:
			{
				auto uniformTexture = static_cast<UniformInfoTexture*>(uniformInfo);
				auto texturePath = uniformTexture->path;
				RHI_Texture* tex = ApplicationBase::Instance()->textureManager->LoadResource(texturePath);
				uniformData->second = make_any<RHI_Texture*>(tex);
				break;
			}
			default:
				DEBUG_LOG_ERROR("Material::PostResourceLoaded UniformInfoType Not Availiable Type:{}", uniformInfo->GetUniformType());
				break;
			}
		}

		// init constantBuffer
		uint32_t size;
		auto value = GetValues4DescriptorSet(size);
		m_valueConstantBuffer = make_shared<RHI_ConstantBuffer>(GetObjectName()+"CBuffer");
		m_valueConstantBuffer->Create(size,1);
		m_valueConstantBuffer->ResetOffset();
		m_valueConstantBuffer->Update(value);

		// modify vertexType
		m_shader->ChangeVertexType(m_materialRes->vertexType);
	}

	void Material::Tick()
	{
		m_isValueDirty = true;
	}
	
	void Material::UpdateValue(const std::string& name)
	{
		EASY_FUNCTION(profiler::colors::Brown200)
		// malloc value 
		if (m_value == nullptr)
		{
			int size = CalcValueSize();
			m_value = malloc(size);
			m_valueSize = size;
			memset(m_value, 0, size);

			DEBUG_LOG_INFO("UpdateValue Malloc Value size:{}", size);
		}

		// check name is valid 
		if (m_uniformDataList.find(name) == m_uniformDataList.end())
		{
			return;
		}

		const auto& value = m_uniformDataList[name];
		if (!value.has_value())
		{
			return;
		}
		const auto& uniformInfo = m_shader->GetGlobalUniformInfo(name);

		int offset = uniformInfo.location;
		int size = uniformInfo.size;
		UniformType uniformType = uniformInfo.type;

		switch (uniformType)
		{
		case UniformType::UNIFORM_BOOL:
		{
			auto boolValue = std::any_cast<bool>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&boolValue), size);
			break;

		}
		case UniformType::UNIFORM_INT:
		{
				auto intValue = std::any_cast<int32_t>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&intValue), size);
			break; }
		case UniformType::UNIFORM_UINT:
		{
			auto uintValue = std::any_cast<uint32_t>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&uintValue), size);
			break;
		}
		case UniformType::UNIFORM_FLOAT:
		{
			auto floatValue = std::any_cast<float>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&floatValue), size);
			break;
		}
		case UniformType::UNIFORM_FLOAT_VEC2:
		{
			auto vec2Value = std::any_cast<Vector2>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&vec2Value), size);
			break;
		}
		case UniformType::UNIFORM_FLOAT_VEC3:
		{
			auto vec3Value = std::any_cast<Vector3>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&vec3Value), size);
			break;
		}
		case UniformType::UNIFORM_FLOAT_VEC4:
		{
			auto vec4Value = std::any_cast<Vector4>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&vec4Value), size);
			break;
		}
		case UniformType::UNIFORM_FLOAT_MAT4:
		{
			auto matrixValue = std::any_cast<Matrix>(value);
			memcpy(reinterpret_cast<std::byte*>(m_value) + offset, reinterpret_cast<std::byte*>(&matrixValue), size);
			break;
		}
		case UniformType::UNIFORM_TEXTURE:
			// texture jump
			break;
		case UniformType::UNIFORM_DOUBLE_MAT4:
		case UniformType::UNIFORM_Struct:
		case UniformType::UNIFORM_Unknown:
		default:
		{
			// DEBUG_LOG_ERROR("UpdateValue no support type name:{} type:{}", name, uniformType);
			break;
		}
		}
	}

	int Material::CalcValueSize()
	{
		return m_shader->GetGlobalSize();
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
