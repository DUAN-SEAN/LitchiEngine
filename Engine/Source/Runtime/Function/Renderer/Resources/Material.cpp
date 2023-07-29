

#include "Material.h"

#include "Loaders/ShaderLoader.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/Buffers/UniformBuffer.h"

void LitchiRuntime::Resource::Material::SetShader(LitchiRuntime::Resource::Shader* p_shader)
{
	m_shader = p_shader;
	if (m_shader)
	{
		UniformBuffer::BindBlockToShader(*m_shader, "EngineUBO");
		FillUniform();
	}
	else
	{
		m_uniformsData.clear();
	}
}

void LitchiRuntime::Resource::Material::FillUniform()
{
	m_uniformsData.clear();

	for (const UniformInfo& element : m_shader->uniforms)
		m_uniformsData.emplace(element.name, element.defaultValue);
}

void LitchiRuntime::Resource::Material::Bind(Texture* p_emptyTexture)
{
	if (HasShader())
	{
		m_shader->Bind();

		int textureSlot = 0;

		for (auto& [name, value] : m_uniformsData)
		{
			auto uniformData = m_shader->GetUniformInfo(name);
			if (uniformData)
			{
				switch (uniformData->type)
				{
				case UniformType::UNIFORM_BOOL:			if (value.type() == typeid(bool))		m_shader->SetUniformInt(name, std::any_cast<bool>(value));			break;
				case UniformType::UNIFORM_INT:			if (value.type() == typeid(int))		m_shader->SetUniformInt(name, std::any_cast<int>(value));			break;
				case UniformType::UNIFORM_FLOAT:		if (value.type() == typeid(float))		m_shader->SetUniformFloat(name, std::any_cast<float>(value));		break;
				case UniformType::UNIFORM_FLOAT_VEC2:	if (value.type() == typeid(glm::vec2))	m_shader->SetUniformVec2(name, std::any_cast<glm::vec2>(value));		break;
				case UniformType::UNIFORM_FLOAT_VEC3:	if (value.type() == typeid(glm::vec3))	m_shader->SetUniformVec3(name, std::any_cast<glm::vec3>(value));		break;
				case UniformType::UNIFORM_FLOAT_VEC4:	if (value.type() == typeid(glm::vec4))	m_shader->SetUniformVec4(name, std::any_cast<glm::vec4>(value));		break;
				case UniformType::UNIFORM_SAMPLER_2D:
				{
					if (value.type() == typeid(Texture*))
					{
						if (auto tex = std::any_cast<Texture*>(value); tex)
						{
							tex->Bind(textureSlot);
							m_shader->SetUniformInt(uniformData->name, textureSlot++);
						}
						else if (p_emptyTexture)
						{
							p_emptyTexture->Bind(textureSlot);
							m_shader->SetUniformInt(uniformData->name, textureSlot++);
						}
					}
				}
				}
			}
		}
	}
}

void LitchiRuntime::Resource::Material::Bind(LitchiRuntime::Texture* p_emptyTexture, Framebuffer4Depth* shadowMapFBO)
{
	if (HasShader())
	{
		m_shader->Bind();

		int textureSlot = 0;

		for (auto& [name, value] : m_uniformsData)
		{
			auto uniformData = m_shader->GetUniformInfo(name);

			if (uniformData)
			{
				switch (uniformData->type)
				{
				case UniformType::UNIFORM_BOOL:			if (value.type() == typeid(bool))		m_shader->SetUniformInt(name, std::any_cast<bool>(value));			break;
				case UniformType::UNIFORM_INT:			if (value.type() == typeid(int))		m_shader->SetUniformInt(name, std::any_cast<int>(value));			break;
				case UniformType::UNIFORM_FLOAT:		if (value.type() == typeid(float))		m_shader->SetUniformFloat(name, std::any_cast<float>(value));		break;
				case UniformType::UNIFORM_FLOAT_VEC2:	if (value.type() == typeid(glm::vec2))	m_shader->SetUniformVec2(name, std::any_cast<glm::vec2>(value));		break;
				case UniformType::UNIFORM_FLOAT_VEC3:	if (value.type() == typeid(glm::vec3))	m_shader->SetUniformVec3(name, std::any_cast<glm::vec3>(value));		break;
				case UniformType::UNIFORM_FLOAT_VEC4:	if (value.type() == typeid(glm::vec4))	m_shader->SetUniformVec4(name, std::any_cast<glm::vec4>(value));		break;
				case UniformType::UNIFORM_SAMPLER_2D:
				{
					if (value.type() == typeid(Texture*))
					{
						if (auto tex = std::any_cast<Texture*>(value); tex)
						{
							tex->Bind(textureSlot);
							m_shader->SetUniformInt(uniformData->name, textureSlot++);
						}
						else if (p_emptyTexture)
						{
							p_emptyTexture->Bind(textureSlot);
							m_shader->SetUniformInt(uniformData->name, textureSlot++);
						}
					}
				}
				}
			}
		}

		// 如果有阴影贴图, 则向shader绑定额外的阴影贴图资源
		if (shadowMapFBO != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + textureSlot);
			glBindTexture(GL_TEXTURE_2D, shadowMapFBO->GetTextureID());

			m_shader->SetUniformInt("u_shadowMap", textureSlot++);
		}
	}
}

void LitchiRuntime::Resource::Material::UnBind()
{
	if (HasShader())
		m_shader->Unbind();
}

LitchiRuntime::Resource::Shader*& LitchiRuntime::Resource::Material::GetShader()
{
	return m_shader;
}

bool LitchiRuntime::Resource::Material::HasShader() const
{
	return m_shader;
}

void LitchiRuntime::Resource::Material::SetBlendable(bool p_transparent)
{
	m_blendable = p_transparent;
}

void LitchiRuntime::Resource::Material::SetBackfaceCulling(bool p_backfaceCulling)
{
	m_backfaceCulling = p_backfaceCulling;
}

void LitchiRuntime::Resource::Material::SetFrontfaceCulling(bool p_frontfaceCulling)
{
	m_frontfaceCulling = p_frontfaceCulling;
}

void LitchiRuntime::Resource::Material::SetDepthTest(bool p_depthTest)
{
	m_depthTest = p_depthTest;
}

void LitchiRuntime::Resource::Material::SetDepthWriting(bool p_depthWriting)
{
	m_depthWriting = p_depthWriting;
}

void LitchiRuntime::Resource::Material::SetColorWriting(bool p_colorWriting)
{
	m_colorWriting = p_colorWriting;
}

void LitchiRuntime::Resource::Material::SetGPUInstances(int p_instances)
{
	m_gpuInstances = p_instances;
}

bool LitchiRuntime::Resource::Material::IsBlendable() const
{
	return m_blendable;
}

bool LitchiRuntime::Resource::Material::HasBackfaceCulling() const
{
	return m_backfaceCulling;
}

bool LitchiRuntime::Resource::Material::HasFrontfaceCulling() const
{
	return m_frontfaceCulling;
}

bool LitchiRuntime::Resource::Material::HasDepthTest() const
{
	return m_depthTest;
}

bool LitchiRuntime::Resource::Material::HasDepthWriting() const
{
	return m_depthWriting;
}

bool LitchiRuntime::Resource::Material::HasColorWriting() const
{
	return m_colorWriting;
}

int LitchiRuntime::Resource::Material::GetGPUInstances() const
{
	return m_gpuInstances;
}

uint8_t LitchiRuntime::Resource::Material::GenerateStateMask() const
{
	uint8_t result = 0;

	if (m_depthWriting)								result |= 0b0000'0001;
	if (m_colorWriting)								result |= 0b0000'0010;
	if (m_blendable)								result |= 0b0000'0100;
	if (m_backfaceCulling || m_frontfaceCulling)	result |= 0b0000'1000;
	if (m_depthTest)								result |= 0b0001'0000;
	if (m_backfaceCulling)							result |= 0b0010'0000;
	if (m_frontfaceCulling)							result |= 0b0100'0000;

	return result;
}

std::map<std::string, std::any>& LitchiRuntime::Resource::Material::GetUniformsData()
{
	return m_uniformsData;
}

void LitchiRuntime::Resource::Material::PostResourceModify()
{
	PostResourceLoaded();

	// 写入本地
	// 获取当前Material句柄在资源管理器的路径, 将句柄写入路径下
}

void LitchiRuntime::Resource::Material::PostResourceLoaded()
{
	// todo 用加载好的资源 初始化运行时字段

	if (materialRes == nullptr || path.empty())
	{
		return;
	}

	// 设置shader
	m_shader = ApplicationBase::Instance()->shaderManager->GetResource(materialRes->shaderPath);
	SetShader(m_shader);

	// 设置setting
	m_backfaceCulling = materialRes->settings.backfaceCulling;
	m_blendable = materialRes->settings.blendable;
	m_depthTest = materialRes->settings.depthTest;
	m_gpuInstances = materialRes->settings.gpuInstances;

	// 设置uniform
	FillUniform();
	for (auto uniformInfo : materialRes->uniformInfoList)
	{
		auto& uniformData = m_uniformsData.find(uniformInfo->name);
		if (uniformData == m_uniformsData.end())
		{
			DEBUG_LOG_ERROR("Material::PostResourceLoaded Not Found Uniform uniformName:{}", uniformInfo->name);
			continue;

		}

		switch (uniformInfo->GetUniformType())
		{
		case UniformInfoType::Bool:
		{
			auto uniformInfoBool = static_cast<UniformInfoBool*>(uniformInfo);
			uniformData->second = std::make_any<bool>(uniformInfoBool->value);
			break;
		}
		case UniformInfoType::Float:
		{
			auto uniformInfoFloat = static_cast<UniformInfoFloat*>(uniformInfo);
			uniformData->second = std::make_any<float>(uniformInfoFloat->value);
			break;
		}
		case UniformInfoType::Int:
		{
			auto uniformInfoInt = static_cast<UniformInfoInt*>(uniformInfo);
			uniformData->second = std::make_any<int>(uniformInfoInt->value);
			break;
		}
		case UniformInfoType::Vector2:
		{
			auto uniformInfoVector2 = static_cast<UniformInfoVector2*>(uniformInfo);
			uniformData->second = std::make_any<glm::vec2>(uniformInfoVector2->vector);
			break;
		}
		case UniformInfoType::Vector3:
		{
			auto uniformInfoVector3 = static_cast<UniformInfoVector3*>(uniformInfo);
			uniformData->second = std::make_any<glm::vec3>(uniformInfoVector3->vector);
			break;
		}
		case UniformInfoType::Vector4:
		{
			auto uniformInfoVector4 = static_cast<UniformInfoVector4*>(uniformInfo);
			uniformData->second = std::make_any<glm::vec4>(uniformInfoVector4->vector);
			break;
		}
		case UniformInfoType::Path:
		{
			auto uniformInfoPath = static_cast<UniformInfoPath*>(uniformInfo);
			auto texture = ApplicationBase::Instance()->textureManager->GetResource(uniformInfoPath->path);
			uniformData->second = std::make_any<Texture*>(texture);
			break;
		}
		default:
			DEBUG_LOG_ERROR("Material::PostResourceLoaded UniformInfoType Not Availiable Type:{}", uniformInfo->GetUniformType());
			break;
		}
	}
}

void LitchiRuntime::Resource::Material::RebuildResourceFromMemory()
{
	if (materialRes == nullptr)
	{
		materialRes = new MaterialRes();
	}

	materialRes->settings.backfaceCulling = m_backfaceCulling;
	materialRes->settings.blendable = m_blendable;
	materialRes->settings.depthTest = m_depthTest;
	materialRes->settings.gpuInstances = m_gpuInstances;
	// other setting

	if (m_shader != nullptr)
	{
		materialRes->shaderPath = m_shader->path;
		// 清理已存在的uniformData
		for (auto value : materialRes->uniformInfoList)
		{
			delete value;
		}
		materialRes->uniformInfoList.clear();

		for (auto& [name, value] : m_uniformsData)
		{
			auto uniformData = m_shader->GetUniformInfo(name);

			if (uniformData)
			{
				switch (uniformData->type)
				{
				case UniformType::UNIFORM_BOOL:
					if (value.type() == typeid(bool))
					{
						auto uniformBool = new UniformInfoBool();
						uniformBool->name = uniformData->name;
						uniformBool->value = std::any_cast<bool>(value);
						materialRes->uniformInfoList.push_back(uniformBool);
					}
					break;
				case UniformType::UNIFORM_INT:
					if (value.type() == typeid(int))
					{
						auto uniformInt = new UniformInfoInt();
						uniformInt->name = uniformData->name;
						uniformInt->value = std::any_cast<int>(value);
						materialRes->uniformInfoList.push_back(uniformInt);

					}
					break;
				case UniformType::UNIFORM_FLOAT:
					if (value.type() == typeid(float))
					{
						auto uniformFloat = new UniformInfoFloat();
						uniformFloat->name = uniformData->name;
						uniformFloat->value = std::any_cast<float>(value);
						materialRes->uniformInfoList.push_back(uniformFloat);
					}
					break;
				case UniformType::UNIFORM_FLOAT_VEC2:
					if (value.type() == typeid(glm::vec2))
					{
						auto uniformVec2 = new UniformInfoVector2();
						uniformVec2->name = uniformData->name;
						uniformVec2->vector = std::any_cast<glm::vec2>(value);
						materialRes->uniformInfoList.push_back(uniformVec2);
					}
					break;
				case UniformType::UNIFORM_FLOAT_VEC3:
					if (value.type() == typeid(glm::vec3))
					{
						auto uniformVec3 = new UniformInfoVector3();
						uniformVec3->name = uniformData->name;
						uniformVec3->vector = std::any_cast<glm::vec3>(value);
						materialRes->uniformInfoList.push_back(uniformVec3);
					}
					break;
				case UniformType::UNIFORM_FLOAT_VEC4:
					if (value.type() == typeid(glm::vec4))
					{
						auto uniformVec4 = new UniformInfoVector4();
						uniformVec4->name = uniformData->name;
						uniformVec4->vector = std::any_cast<glm::vec4>(value);
						materialRes->uniformInfoList.push_back(uniformVec4);
					}
					break;
				case UniformType::UNIFORM_SAMPLER_2D:
					if (value.type() == typeid(Texture*))
					{
						auto uniformPath = new UniformInfoPath();
						uniformPath->name = uniformData->name;
						uniformPath->path = "";
						if (auto tex = std::any_cast<Texture*>(value); tex)
						{
							uniformPath->path = tex->path;
						}
						materialRes->uniformInfoList.push_back(uniformPath);
					}
					break;
				default:
					DEBUG_LOG_ERROR("RebuildResourceFromMemory Not Found UniformType type:{}", uniformData->type);
					break;
				}
			}
		}

	}


}
