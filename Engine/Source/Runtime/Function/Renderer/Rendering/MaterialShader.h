
#pragma once

//= INCLUDES ======================
#include "../RHI/RHI_Definitions.h"
#include "../Resource/IResource.h"
#include "Runtime/Function/Renderer/Resources/ShaderUniform.h"
#include "Runtime/Function/Renderer/RHI/RHI_Descriptor.h"
//=================================

namespace LitchiRuntime
{

	/* composite RHI_Shader Group */
	struct MaterialShader
	{
		MaterialShader() = default;

		MaterialShader(std::string shaderPath, RHI_Shader* vertex_shader, RHI_Shader* pixel_shader):
			m_invalidTextureDescriptor()
		{
			m_shaderPath = shaderPath;
			m_vertex_shader = vertex_shader;
			m_pixel_shader = pixel_shader;
		}

		//= IResource ===========================================
		bool LoadFromFile(const std::string& file_path);
		bool SaveToFile(const std::string& file_path);

		/* Global Uniform  */
		const ShaderUniform GetGlobalUniformInfo(std::string name);
		const std::vector<ShaderUniform> GetGlobalShaderUniformList();
		const RHI_Descriptor& GetTextureDescriptor(std::string name);
		const std::vector<RHI_Descriptor> GetTextureDescriptorList();
		int GetGlobalSize();

		std::string m_shaderPath;

		/* shader */
		RHI_Shader* m_vertex_shader;
		RHI_Shader* m_pixel_shader;

		RHI_Descriptor m_invalidTextureDescriptor;
	};

}
