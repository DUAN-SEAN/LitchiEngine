
#include <glad/glad.h>
#include "Framebuffer4Depth.h"


LitchiRuntime::Framebuffer4Depth::Framebuffer4Depth(uint16_t p_width, uint16_t p_height)
{
	/* Generate OpenGL objects */
	glGenFramebuffers(1, &m_bufferID);
	glGenTextures(1, &m_renderTexture);

	/* Setup texture */
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Setup framebuffer */
	Bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	Unbind();

	Resize(p_width, p_height);
}

LitchiRuntime::Framebuffer4Depth::~Framebuffer4Depth()
{
	/* Destroy OpenGL objects */
	glDeleteBuffers(1, &m_bufferID);
	glDeleteTextures(1, &m_renderTexture);
}

void LitchiRuntime::Framebuffer4Depth::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
}

void LitchiRuntime::Framebuffer4Depth::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LitchiRuntime::Framebuffer4Depth::Resize(uint16_t p_width, uint16_t p_height)
{
	/* Resize texture */
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p_width, p_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t LitchiRuntime::Framebuffer4Depth::GetID()
{
	return m_bufferID;
}

uint32_t LitchiRuntime::Framebuffer4Depth::GetTextureID()
{
	return m_renderTexture;
}
