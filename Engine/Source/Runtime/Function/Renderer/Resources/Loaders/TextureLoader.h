
#pragma once

#include <string>
#include <vector>

#include "Runtime/Function/Renderer/Settings/ETextureFilteringMode.h"

namespace LitchiRuntime
{
	class Texture;
}

namespace LitchiRuntime::Loaders
{
	/**
	* Handle the Texture creation and destruction
	*/
	class TextureLoader
	{
	public:
		/**
		* Disabled constructor
		*/
		TextureLoader() = delete;

		/**
		* Create a texture from file
		* @param p_filePath
		* @param p_firstFilter
		* @param p_secondFilter
		* @param p_generateMipmap
		*/
		static Texture* Create(const std::string& p_filepath, ETextureFilteringMode p_firstFilter, ETextureFilteringMode p_secondFilter, bool p_generateMipmap);

		/**
		* Create a texture from a single pixel color
		* @param p_data
		* @param p_firstFilder
		* @param p_secondFilter
		* @param p_generateMipmap
		*/
		static Texture* CreateColor(uint32_t p_data, ETextureFilteringMode p_firstFilter, ETextureFilteringMode p_secondFilter, bool p_generateMipmap);

		/**
		* Create a texture from memory
		* @param p_data
		* @param p_width
		* @param p_height
		* @param p_firstFilder
		* @param p_secondFilter
		* @param p_generateMipmap
		*/
		static Texture* CreateFromMemory(uint8_t* p_data, uint32_t p_width, uint32_t p_height, ETextureFilteringMode p_firstFilter, ETextureFilteringMode p_secondFilter, bool p_generateMipmap);

		/**
		* Create a texture from memory
		* @param p_data
		* @param p_width
		* @param p_height
		* @param p_firstFilder
		* @param p_secondFilter
		* @param p_generateMipmap
		* @param p_server_format
		* @param p_client_format
		* @param p_data_type
		*/
		static Texture* CreateFromMemory(uint8_t* p_data, uint32_t p_width, uint32_t p_height, ETextureFilteringMode p_firstFilter, ETextureFilteringMode p_secondFilter, bool p_generateMipmap, unsigned int p_server_format, unsigned int p_client_format, unsigned int p_data_type);

		/**
		* Reload a texture from file
		* @param p_texture
		* @param p_filePath
		* @param p_firstFilter
		* @param p_secondFilter
		* @param p_generateMipmap
		*/
		static void Reload(Texture& p_texture, const std::string& p_filePath, ETextureFilteringMode p_firstFilter, ETextureFilteringMode p_secondFilter, bool p_generateMipmap);

		/**
		* Destroy a texture
		* @param p_textureInstance
		*/
		static bool Destroy(Texture*& p_textureInstance);
	};
}
