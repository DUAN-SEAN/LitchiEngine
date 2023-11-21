
//= INCLUDES =========================
#include "MaterialShader.h"
#include "Runtime/Core/pch.h"
#include "Runtime/Function/Renderer/RHI/RHI_Shader.h"
//====================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
	bool MaterialShader::LoadFromFile(const std::string& file_path)
	{
		// todo: Ä¬ÈÏ¶¼ÊÇ
		auto vertexShader = new RHI_Shader();
		vertexShader->Compile(RHI_Shader_Stage::RHI_Shader_Vertex, file_path, false, RHI_Vertex_Type::PosUvNorTan);
		auto pixelShader = new RHI_Shader();
		pixelShader->Compile(RHI_Shader_Stage::RHI_Shader_Pixel, file_path, false, RHI_Vertex_Type::PosUvNorTan);

		m_shaderPath = FileSystem::GetRelativePathAssetFromNative(file_path);
		m_vertex_shader = vertexShader;
		m_pixel_shader = pixelShader;

		return true;
	}
	bool MaterialShader::SaveToFile(const std::string& file_path)
	{
		return true;
	}
	const ShaderUniform MaterialShader::GetGlobalUniformInfo(std::string name)
	{
		const auto& descriptor = m_vertex_shader->GetGlobalDescriptor();
		const auto& uniformList = descriptor.uniformList[0].memberUniform;

		for (auto uniform : uniformList)
		{
			if (uniform.name == name)
			{
				return uniform;
			}
		}

		return ShaderUniform();
	}

	const std::vector<ShaderUniform> MaterialShader::GetGlobalShaderUniformList()
	{
		const auto& descriptor = m_vertex_shader->GetGlobalDescriptor();
		const auto& uniformList = descriptor.uniformList[0].memberUniform;
		return uniformList;
	}

	const RHI_Descriptor& MaterialShader::GetTextureDescriptor(std::string name)
	{
		auto& descriptor = m_vertex_shader->GetDescriptors();

		for (auto& rhi_descriptor : descriptor)
		{
			if (rhi_descriptor.type == RHI_Descriptor_Type::Texture && rhi_descriptor.name == name)
			{
				return rhi_descriptor;
			}
		}

		return m_invalidTextureDescriptor;
	}

	const std::vector<RHI_Descriptor> MaterialShader::GetTextureDescriptorList()
	{
		std::vector<RHI_Descriptor> descriptorList;
		auto& descriptor = m_vertex_shader->GetDescriptors();

		for (auto rhi_descriptor : descriptor)
		{
			if (rhi_descriptor.type == RHI_Descriptor_Type::Texture)
			{
				descriptorList.push_back(rhi_descriptor);
			}
		}

		return descriptorList;
	}

	int MaterialShader::GetGlobalSize()
	{
		if (m_vertex_shader)
		{
			const auto& descriptor = m_vertex_shader->GetGlobalDescriptor();
			return descriptor.struct_size;
		}

		return 0;
	}
}
