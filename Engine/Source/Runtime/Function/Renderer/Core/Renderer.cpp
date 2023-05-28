
#include <glad/glad.h>

#include "Renderer.h"

#include "Runtime/Core/App/application_base.h"
#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Function/Renderer/Resources/Loaders/TextureLoader.h"

LitchiRuntime::Renderer::Renderer(Driver& p_driver) :
	m_driver(p_driver),
	m_state(0),
	m_emptyTexture(Loaders::TextureLoader::CreateColor
	(
		(255 << 24) | (255 << 16) | (255 << 8) | 255,
		ETextureFilteringMode::NEAREST,
		ETextureFilteringMode::NEAREST,
		false
	))
{
}

void LitchiRuntime::Renderer::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha)
{
	glClearColor(p_red, p_green, p_blue, p_alpha);
}

void LitchiRuntime::Renderer::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer)
{
	glClear
	(
		(p_colorBuffer ? GL_COLOR_BUFFER_BIT : 0) |
		(p_depthBuffer ? GL_DEPTH_BUFFER_BIT : 0) |
		(p_stencilBuffer ? GL_STENCIL_BUFFER_BIT : 0)
	);
}

void LitchiRuntime::Renderer::Clear(RenderCamera & p_camera, bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer)
{
	/* Backup the previous OpenGL clear color */
	GLfloat previousClearColor[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, previousClearColor);

	/* Clear the screen using the camera clear color */
	const glm::vec3& cameraClearColor = p_camera.GetClearColor();
	SetClearColor(cameraClearColor.x, cameraClearColor.y, cameraClearColor.z, 1.0f);
	Clear(p_colorBuffer, p_depthBuffer, p_stencilBuffer);

	/* Reset the OpenGL clear color to the previous clear color (Backuped one) */
	SetClearColor(previousClearColor[0], previousClearColor[1], previousClearColor[2], previousClearColor[3]);
}

void LitchiRuntime::Renderer::SetRasterizationLinesWidth(float p_width)
{
	glLineWidth(p_width);
}

void LitchiRuntime::Renderer::SetRasterizationMode(LitchiRuntime::ERasterizationMode p_rasterizationMode)
{
	glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(p_rasterizationMode));
}

void LitchiRuntime::Renderer::SetCapability(LitchiRuntime::ERenderingCapability p_capability, bool p_value)
{
	(p_value ? glEnable : glDisable)(static_cast<GLenum>(p_capability));
}

bool LitchiRuntime::Renderer::GetCapability(LitchiRuntime::ERenderingCapability p_capability) const
{
	return glIsEnabled(static_cast<GLenum>(p_capability));
}

void LitchiRuntime::Renderer::SetStencilAlgorithm(LitchiRuntime::EComparaisonAlgorithm p_algorithm, int32_t p_reference, uint32_t p_mask)
{
	glStencilFunc(static_cast<GLenum>(p_algorithm), p_reference, p_mask);
}

void LitchiRuntime::Renderer::SetDepthAlgorithm(LitchiRuntime::EComparaisonAlgorithm p_algorithm)
{
	glDepthFunc(static_cast<GLenum>(p_algorithm));
}

void LitchiRuntime::Renderer::SetStencilMask(uint32_t p_mask)
{
	glStencilMask(p_mask);
}

void LitchiRuntime::Renderer::SetStencilOperations(LitchiRuntime::EOperation p_stencilFail, LitchiRuntime::EOperation p_depthFail, LitchiRuntime::EOperation p_bothPass)
{
	glStencilOp(static_cast<GLenum>(p_stencilFail), static_cast<GLenum>(p_depthFail), static_cast<GLenum>(p_bothPass));
}

void LitchiRuntime::Renderer::SetCullFace(LitchiRuntime::ECullFace p_cullFace)
{
	glCullFace(static_cast<GLenum>(p_cullFace));
}

void LitchiRuntime::Renderer::SetDepthWriting(bool p_enable)
{
	glDepthMask(p_enable);
}

void LitchiRuntime::Renderer::SetColorWriting(bool p_enableRed, bool p_enableGreen, bool p_enableBlue, bool p_enableAlpha)
{
	glColorMask(p_enableRed, p_enableGreen, p_enableBlue, p_enableAlpha);
}

void LitchiRuntime::Renderer::SetColorWriting(bool p_enable)
{
	glColorMask(p_enable, p_enable, p_enable, p_enable);
}

void LitchiRuntime::Renderer::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void LitchiRuntime::Renderer::ReadPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height, LitchiRuntime::EPixelDataFormat format, LitchiRuntime::EPixelDataType type, void* data)
{
	glReadPixels(x, y, width, height, static_cast<GLenum>(format), static_cast<GLenum>(type), data);
}

bool LitchiRuntime::Renderer::GetBool(GLenum p_parameter)
{
	GLboolean result;
	glGetBooleanv(p_parameter, &result);
	return static_cast<bool>(result);
}

bool LitchiRuntime::Renderer::GetBool(GLenum p_parameter, uint32_t p_index)
{
	GLboolean result;
	glGetBooleani_v(p_parameter, p_index, &result);
	return static_cast<bool>(result);
}

int LitchiRuntime::Renderer::GetInt(GLenum p_parameter)
{
	GLint result;
	glGetIntegerv(p_parameter, &result);
	return static_cast<int>(result);
}

int LitchiRuntime::Renderer::GetInt(GLenum p_parameter, uint32_t p_index)
{
	GLint result;
	glGetIntegeri_v(p_parameter, p_index, &result);
	return static_cast<int>(result);
}

float LitchiRuntime::Renderer::GetFloat(GLenum p_parameter)
{
	GLfloat result;
	glGetFloatv(p_parameter, &result);
	return static_cast<float>(result);
}

float LitchiRuntime::Renderer::GetFloat(GLenum p_parameter, uint32_t p_index)
{
	GLfloat result;
	glGetFloati_v(p_parameter, p_index, &result);
	return static_cast<float>(result);
}

double LitchiRuntime::Renderer::GetDouble(GLenum p_parameter)
{
	GLdouble result;
	glGetDoublev(p_parameter, &result);
	return static_cast<double>(result);
}

double LitchiRuntime::Renderer::GetDouble(GLenum p_parameter, uint32_t p_index)
{
	GLdouble result;
	glGetDoublei_v(p_parameter, p_index, &result);
	return static_cast<double>(result);
}

int64_t LitchiRuntime::Renderer::GetInt64(GLenum p_parameter)
{
	GLint64 result;
	glGetInteger64v(p_parameter, &result);
	return static_cast<int64_t>(result);
}

int64_t LitchiRuntime::Renderer::GetInt64(GLenum p_parameter, uint32_t p_index)
{
	GLint64 result;
	glGetInteger64i_v(p_parameter, p_index, &result);
	return static_cast<int64_t>(result);
}

std::string LitchiRuntime::Renderer::GetString(GLenum p_parameter)
{
	const GLubyte* result = glGetString(p_parameter);
	return result ? reinterpret_cast<const char*>(result) : std::string();
}

std::string LitchiRuntime::Renderer::GetString(GLenum p_parameter, uint32_t p_index)
{
	const GLubyte* result = glGetStringi(p_parameter, p_index);
	return result ? reinterpret_cast<const char*>(result) : std::string();
}

void LitchiRuntime::Renderer::ClearFrameInfo()
{
	m_frameInfo.batchCount		= 0;
	m_frameInfo.instanceCount	= 0;
	m_frameInfo.polyCount		= 0;
}

void LitchiRuntime::Renderer::DrawMesh(Mesh& p_mesh, Resource::Material& p_material, glm::mat4 const* p_modelMatrix)
{
	if (p_material.HasShader() && p_material.GetGPUInstances() > 0)
	{
		if (p_modelMatrix)
			ApplicationBase::Instance()->engineUBO->SetSubData(*p_modelMatrix, 0);
			// m_modelMatrixSender(*p_modelMatrix);

		uint8_t stateMask = p_material.GenerateStateMask();
		ApplyStateMask(stateMask);

		/* Draw the mesh */
		p_material.Bind(m_emptyTexture);
		Draw(p_mesh, EPrimitiveMode::TRIANGLES, p_material.GetGPUInstances());
		p_material.UnBind();
	}
}

void LitchiRuntime::Renderer::Draw(LitchiRuntime::IMesh& p_mesh, LitchiRuntime::EPrimitiveMode p_primitiveMode, uint32_t p_instances)
{
	if (p_instances > 0)
	{
		++m_frameInfo.batchCount;
		m_frameInfo.instanceCount += p_instances;
		m_frameInfo.polyCount += (p_mesh.GetIndexCount() / 3) * p_instances;

		p_mesh.Bind();
		
		if (p_mesh.GetIndexCount() > 0)
		{
			/* With EBO */
			if (p_instances == 1)
				glDrawElements(static_cast<GLenum>(p_primitiveMode), p_mesh.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
			else
				glDrawElementsInstanced(static_cast<GLenum>(p_primitiveMode), p_mesh.GetIndexCount(), GL_UNSIGNED_INT, nullptr, p_instances);
		}
		else
		{
			/* Without EBO */
			if (p_instances == 1)
				glDrawArrays(static_cast<GLenum>(p_primitiveMode), 0, p_mesh.GetVertexCount());
			else
				glDrawArraysInstanced(static_cast<GLenum>(p_primitiveMode), 0, p_mesh.GetVertexCount(), p_instances);
		}

		p_mesh.Unbind();
	}
}

//std::vector<std::reference_wrapper<LitchiRuntime::Mesh>> LitchiRuntime::Renderer::GetMeshesInFrustum
//(
//	const LitchiRuntime::Model& p_model,
//	const ::BoundingSphere& p_modelBoundingSphere,
//	const OvMaths::FTransform& p_modelTransform,
//	const Data::Frustum& p_frustum,
//	LitchiRuntime::ECullingOptions p_cullingOptions
//)
//{
//	const bool frustumPerModel = Settings::IsFlagSet(LitchiRuntime::ECullingOptions::FRUSTUM_PER_MODEL, p_cullingOptions);
//
//	if (!frustumPerModel || p_frustum.BoundingSphereInFrustum(p_modelBoundingSphere, p_modelTransform))
//	{
//		std::vector<std::reference_wrapper<LitchiRuntime::Mesh>> result;
//
//		const bool frustumPerMesh = Settings::IsFlagSet(LitchiRuntime::ECullingOptions::FRUSTUM_PER_MESH, p_cullingOptions);
//
//		const auto& meshes = p_model.GetMeshes();
//
//		for (auto mesh : meshes)
//		{
//			// Do not check if the mesh is in frustum if the model has only one mesh, because model and mesh bounding sphere are equals
//			if (meshes.size() == 1 || !frustumPerMesh || p_frustum.BoundingSphereInFrustum(mesh->GetBoundingSphere(), p_modelTransform))
//			{
//				result.emplace_back(*mesh);
//			}
//		}
//
//		return result;
//	}
//
//	return {};
//}

uint8_t LitchiRuntime::Renderer::FetchGLState()
{
	uint8_t result = 0;

	GLboolean cMask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, cMask);

	if (GetBool(GL_DEPTH_WRITEMASK))						result |= 0b0000'0001;
	if (cMask[0])											result |= 0b0000'0010;
	if (GetCapability(ERenderingCapability::BLEND))			result |= 0b0000'0100;
	if (GetCapability(ERenderingCapability::CULL_FACE))		result |= 0b0000'1000;
	if (GetCapability(ERenderingCapability::DEPTH_TEST))	result |= 0b0001'0000;

	switch (static_cast<ECullFace>(GetInt(GL_CULL_FACE)))
	{
	case LitchiRuntime::ECullFace::BACK:			result |= 0b0010'0000; break;
	case LitchiRuntime::ECullFace::FRONT:			result |= 0b0100'0000; break;
	case LitchiRuntime::ECullFace::FRONT_AND_BACK:  result |= 0b0110'0000; break;
	}

	return result;
}

void LitchiRuntime::Renderer::ApplyStateMask(uint8_t p_mask)
{
	if (p_mask != m_state)
	{
		if ((p_mask & 0x01) != (m_state & 0x01))	SetDepthWriting(p_mask & 0x01);
		if ((p_mask & 0x02) != (m_state & 0x02))	SetColorWriting(p_mask & 0x02);
		if ((p_mask & 0x04) != (m_state & 0x04))	SetCapability(ERenderingCapability::BLEND, p_mask & 0x04);
		if ((p_mask & 0x08) != (m_state & 0x08))	SetCapability(ERenderingCapability::CULL_FACE, p_mask & 0x8);
		if ((p_mask & 0x10) != (m_state & 0x10))	SetCapability(ERenderingCapability::DEPTH_TEST, p_mask & 0x10);

		if ((p_mask & 0x08) && ((p_mask & 0x20) != (m_state & 0x20) || (p_mask & 0x40) != (m_state & 0x40)))
		{
			int backBit = p_mask & 0x20;
			int frontBit = p_mask & 0x40;
			SetCullFace(backBit && frontBit ? ECullFace::FRONT_AND_BACK : (backBit ? ECullFace::BACK : ECullFace::FRONT));
		}

		m_state = p_mask;
	}
}

void LitchiRuntime::Renderer::SetState(uint8_t p_state)
{
	m_state = p_state;
}

const LitchiRuntime::Renderer::FrameInfo& LitchiRuntime::Renderer::GetFrameInfo() const
{
	return m_frameInfo;
}
