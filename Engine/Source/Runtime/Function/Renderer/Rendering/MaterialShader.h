
#pragma once

//= INCLUDES ======================
#include "../RHI/RHI_Definitions.h"
#include "../Resource/IResource.h"
#include "ShaderUniform.h"
#include "Runtime/Function/Renderer/RHI/RHI_Descriptor.h"
//=================================

namespace LitchiRuntime
{

	/* composite RHI_Shader Group */
	struct MaterialShader
	{
		MaterialShader() = default;

		MaterialShader(std::string shaderPath, RHI_Shader* vertex_shader, RHI_Shader* pixel_shader):
			m_invalidTextureDescriptor(), m_globalMaterial{}
		{
			m_shaderPath = shaderPath;
			m_vertex_shader = vertex_shader;
			m_pixel_shader = pixel_shader;
			LoadMaterialDescriptors();
		}

		//= IResource ===========================================
		bool LoadFromFile(const std::string& file_path);
		bool SaveToFile(const std::string& file_path);
		void LoadMaterialDescriptors();

		// default type=PosUvNorTan
		void ChangeVertexType(RHI_Vertex_Type vertexType);

		/* Global Uniform  */
		const ShaderUniform& GetGlobalUniformInfo(std::string name);
		const std::unordered_map<std::string, ShaderUniform>& GetGlobalShaderUniformDict();
		const RHI_Descriptor& GetTextureDescriptor(std::string name);
		const std::unordered_map<std::string, RHI_Descriptor>& GetTextureDescriptorDict();
		std::vector<RHI_Descriptor>& GetMaterialDescriptors() { return m_materialDescriptors; }
		int GetGlobalSize();

		std::string m_shaderPath;

		/* shader */
		RHI_Shader* m_vertex_shader;
		RHI_Shader* m_pixel_shader;

		RHI_Descriptor m_globalMaterial;
		std::unordered_map<std::string, ShaderUniform> m_globalUniformDict;
		std::unordered_map<std::string, RHI_Descriptor> m_textureDescriptorDict;

		// both vertex and pixel descriptor
		std::vector<RHI_Descriptor> m_materialDescriptors;

		RHI_Descriptor m_invalidTextureDescriptor;
	};

}
