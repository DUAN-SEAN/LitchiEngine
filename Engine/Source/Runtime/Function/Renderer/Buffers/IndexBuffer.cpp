
#include "IndexBuffer.h"
#include <glad/glad.h>

LitchiRuntime::IndexBuffer::IndexBuffer(unsigned int* p_data, size_t p_elements)
{
	glGenBuffers(1, &m_bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_elements * sizeof(unsigned int), p_data, GL_STATIC_DRAW);
}

LitchiRuntime::IndexBuffer::IndexBuffer(std::vector<uint32_t>& p_data) : IndexBuffer(p_data.data(), p_data.size())
{
}

LitchiRuntime::IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_bufferID);
}

void LitchiRuntime::IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
}

void LitchiRuntime::IndexBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t LitchiRuntime::IndexBuffer::GetID()
{
	return m_bufferID;
}
