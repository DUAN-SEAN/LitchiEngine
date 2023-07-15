
#pragma once

#include <stdint.h>
#include <string>

#include "Runtime/Function/Renderer/Settings/ETextureFilteringMode.h"


namespace LitchiRuntime
{
	namespace Loaders { class TextureLoader; }

	/**
	* OpenGL texture wrapper
	*/
	class Texture
	{
		friend class Loaders::TextureLoader;

	public:
		/**
		* Bind the texture to the given slot
		* @param p_slot
		*/
		void Bind(uint32_t p_slot = 0) const;

		/**
		* Unbind the texture
		*/
		void Unbind() const;

		void UpdateSubImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type, unsigned char* data);
		void UpdateImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type, unsigned char* data);
	private:
		Texture(const std::string p_path, uint32_t p_id, uint32_t p_width, uint32_t p_height, uint32_t p_bpp, ETextureFilteringMode p_firstFilter, ETextureFilteringMode p_secondFilter, bool p_generateMipmap);
		~Texture() = default;



	public:
		const uint32_t id;
		const uint32_t width;
		const uint32_t height;
		const uint32_t bitsPerPixel;
		const ETextureFilteringMode firstFilter;
		const ETextureFilteringMode secondFilter;
		const std::string path;
		const bool isMimapped;


		unsigned int gl_texture_format_;
	};
}
