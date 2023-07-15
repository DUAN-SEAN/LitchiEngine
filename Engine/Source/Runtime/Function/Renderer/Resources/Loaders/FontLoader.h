
#pragma once

#include <string>
#include <vector>

#include "Runtime/Function/Renderer/Settings/ETextureFilteringMode.h"

namespace LitchiRuntime
{
	class Font;
}

namespace LitchiRuntime::Loaders
{
	/**
	* Handle the Font creation and destruction
	*/
	class FontLoader
	{
	public:
		/**
		* Disabled constructor
		*/
		FontLoader() = delete;

		/**
		* Create a Font from file
		* @param p_filepath
		* @param p_font_size
		*/
		static Font* Create(const std::string& p_filepath, unsigned short p_font_size);
		
	};
}
