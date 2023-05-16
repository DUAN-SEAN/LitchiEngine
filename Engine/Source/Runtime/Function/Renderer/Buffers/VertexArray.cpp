
#include <glad/glad.h>

#include "VertexArray.h"

LitchiRuntime::VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_bufferID);
	glBindVertexArray(m_bufferID);
}

LitchiRuntime::VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_bufferID);
}

void LitchiRuntime::VertexArray::Bind()
{
	glBindVertexArray(m_bufferID);
}

void LitchiRuntime::VertexArray::Unbind()
{
	glBindVertexArray(0);
}

GLint LitchiRuntime::VertexArray::GetID()
{
	return m_bufferID;
}
