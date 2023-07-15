
#include "Texture.h"

#include "texture2d.h"
#include "Runtime/Core/Log/debug.h"

void LitchiRuntime::Texture::Bind(uint32_t p_slot) const
{
	glActiveTexture(GL_TEXTURE0 + p_slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

void LitchiRuntime::Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

LitchiRuntime::Texture::Texture(const std::string p_path, uint32_t p_id, uint32_t p_width, uint32_t p_height, uint32_t p_bpp, ETextureFilteringMode p_firstFilter, ETextureFilteringMode p_secondFilter, bool p_generateMipmap) : path(p_path),
	id(p_id), width(p_width), height(p_height), bitsPerPixel(p_bpp), firstFilter(p_firstFilter), secondFilter(p_secondFilter), isMimapped(p_generateMipmap)
{

}

void LitchiRuntime::Texture::UpdateSubImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type, unsigned char* data)
{
	if (width <= 0 || height <= 0) {
		return;
	}
	glBindTexture(GL_TEXTURE_2D, this->id); __CHECK_GL_ERROR__
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); __CHECK_GL_ERROR__
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, client_format, data_type, data); __CHECK_GL_ERROR__

}

void LitchiRuntime::Texture::UpdateImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type, unsigned char* data)
{
	if (width <= 0 || height <= 0) {
		return;
	}
	glBindTexture(GL_TEXTURE_2D, this->id); __CHECK_GL_ERROR__
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); __CHECK_GL_ERROR__
	glTexImage2D(GL_TEXTURE_2D, 0, gl_texture_format_, width, height, 0, client_format, data_type, data); __CHECK_GL_ERROR__
}
