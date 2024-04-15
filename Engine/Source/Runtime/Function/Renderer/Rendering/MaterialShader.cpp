
//= INCLUDES =========================
#include "MaterialShader.h"
#include "Runtime/Core/pch.h"
#include "Runtime/Function/Renderer/RHI/RHI_InputLayout.h"
#include "Runtime/Function/Renderer/RHI/RHI_Shader.h"
//====================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
	MaterialShader::MaterialShader(std::string shaderPath, RHI_Shader* vertex_shader, RHI_Shader* pixel_shader) :
		m_invalidTextureDescriptor(), m_globalMaterial{}
	{
		m_shaderPath = shaderPath;
		m_vertex_shader = vertex_shader;
		m_pixel_shader = pixel_shader;

		if (m_vertex_shader)
		{
			m_hash = rhi_hash_combine(m_hash, m_vertex_shader->GetHash());
		}

		if (m_pixel_shader)
		{
			m_hash = rhi_hash_combine(m_hash, m_pixel_shader->GetHash());
		}

		LoadMaterialDescriptors();
	}

	bool MaterialShader::LoadFromFile(const std::string& file_path)
	{
		auto vertexShader = new RHI_Shader();
		vertexShader->Compile(RHI_Shader_Stage::RHI_Shader_Vertex, file_path, false, RHI_Vertex_Type::PosUvNorTan);
		DEBUG_LOG_INFO("GetGlobalDescriptor descriptorSize:{}", vertexShader->GetDescriptors().size());
		auto pixelShader = new RHI_Shader();
		pixelShader->Compile(RHI_Shader_Stage::RHI_Shader_Pixel, file_path, false, RHI_Vertex_Type::PosUvNorTan);

		m_shaderPath = FileSystem::GetRelativePathAssetFromNative(file_path);
		m_vertex_shader = vertexShader;
		m_pixel_shader = pixelShader;
		if(!LoadMaterialDescriptors())
		{
			DEBUG_LOG_ERROR("MaterialShader::LoadMaterialDescriptors Fail :{}", file_path);
			return false;
		}

		DEBUG_LOG_INFO("MaterialShader::LoadFromFile Done :{}", file_path);
		return true;
	}
	bool MaterialShader::SaveToFile(const std::string& file_path)
	{
		return true;
	}
	bool MaterialShader::LoadMaterialDescriptors()
	{
		// load material desc
		bool isGlobalMaterial = m_vertex_shader->GetGlobalDescriptor(m_globalMaterial);
		if(!isGlobalMaterial)
		{
			isGlobalMaterial = m_pixel_shader->GetGlobalDescriptor(m_globalMaterial);
			if(!isGlobalMaterial)
			{
				return false;
			}
		}

		auto uniformList = *(m_globalMaterial.uniformList->at(0).memberUniform);
		for (auto uniform : uniformList)
		{
			DEBUG_LOG_INFO("MaterialShader::LoadMaterialDescriptors globalUniform name :{}", uniform.name);
			m_globalUniformDict[uniform.name] = uniform;
		}

		// ¼ÓÔØÌùÍ¼
		auto& descriptor = m_vertex_shader->GetDescriptors();

		for (auto& rhi_descriptor : descriptor)
		{
			if (rhi_descriptor.type == RHI_Descriptor_Type::Texture && rhi_descriptor.slot >= rhi_shader_shift_register_material_t)
			{
				m_textureDescriptorDict[rhi_descriptor.name] = rhi_descriptor;
			}
		}
		if(m_pixel_shader)
		{
			auto& pixelDescriptors = m_pixel_shader->GetDescriptors();

			for (auto& rhi_descriptor : pixelDescriptors)
			{
				if (rhi_descriptor.type == RHI_Descriptor_Type::Texture && rhi_descriptor.slot >= rhi_shader_shift_register_material_t)
				{
					m_textureDescriptorDict[rhi_descriptor.name] = rhi_descriptor;
				}
			}
		}
		for (auto desc: m_textureDescriptorDict)
		{
			DEBUG_LOG_INFO("MaterialShader::LoadMaterialDescriptors m_textureDescriptorDict name :{}", desc.first);
		}

		DEBUG_LOG_INFO("LoadMaterialDescriptors3:{}", m_shaderPath);
		// ºÏ²¢desc
		m_materialDescriptors = m_vertex_shader->GetDescriptors();
		bool isNeedSort = false;
		// If there is a pixel shader, merge it's resources into our map as well
		if (m_pixel_shader)
		{
			for (const RHI_Descriptor& descriptor_pixel : m_pixel_shader->GetDescriptors())
			{
				// Assume that the descriptor has been created in the vertex shader and only try to update it's shader stage
				bool updated_existing = false;
				for (RHI_Descriptor& descriptor_vertex : m_materialDescriptors)
				{
					if (descriptor_vertex.slot == descriptor_pixel.slot)
					{
						descriptor_vertex.stage |= descriptor_pixel.stage;
						updated_existing = true;
						break;
					}
				}

				// If no updating took place, this a pixel shader only resource, add it
				if (!updated_existing)
				{
					isNeedSort = true;
					m_materialDescriptors.emplace_back(descriptor_pixel);
				}
			}
		}if (isNeedSort)
		{
			// sort descriptors by slot, this is because dynamic offsets (which are computed in a serialized
			// manner in RHI_DescriptorSetLayout::GetDynamicOffsets(), need to be ordered by their slot
			std::sort(m_materialDescriptors.begin(), m_materialDescriptors.end(), [](const RHI_Descriptor& a, const RHI_Descriptor& b)
				{
					return a.slot < b.slot;
				});
		}

		return true;
	}
	void MaterialShader::ChangeVertexType(RHI_Vertex_Type vertexType)
	{
		if(m_vertex_shader)
		{
			m_vertex_shader->GetInputLayout()->Create(vertexType);
		}
	}

	const ShaderUniform& MaterialShader::GetGlobalUniformInfo(std::string name)
	{
		return m_globalUniformDict[name];
	}

	const std::unordered_map<std::string, ShaderUniform>& MaterialShader::GetGlobalShaderUniformDict()
	{
		return m_globalUniformDict;
	}

	const RHI_Descriptor& MaterialShader::GetTextureDescriptor(std::string name)
	{
		return m_textureDescriptorDict[name];
	}

	const std::unordered_map<std::string, RHI_Descriptor>& MaterialShader::GetTextureDescriptorDict()
	{
		return m_textureDescriptorDict;
	}

	int MaterialShader::GetGlobalSize()
	{
		return m_globalMaterial.struct_size;
	}
}
