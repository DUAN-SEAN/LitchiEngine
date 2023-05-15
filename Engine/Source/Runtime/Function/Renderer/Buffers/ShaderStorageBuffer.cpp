
#include "ShaderStorageBuffer.h"

#include <glad/glad.h>

LitchiRuntime::ShaderStorageBuffer::ShaderStorageBuffer(EAccessSpecifier p_accessSpecifier)
{
	glGenBuffers(1, &m_bufferID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, static_cast<GLenum>(p_accessSpecifier));
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_bufferID);
}

LitchiRuntime::ShaderStorageBuffer::~ShaderStorageBuffer()
{
	glDeleteBuffers(1, &m_bufferID);
}

void LitchiRuntime::ShaderStorageBuffer::Bind(uint32_t p_bindingPoint)
{
	m_bindingPoint = p_bindingPoint;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, p_bindingPoint, m_bufferID);
}

void LitchiRuntime::ShaderStorageBuffer::Unbind()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_bindingPoint, 0);
}