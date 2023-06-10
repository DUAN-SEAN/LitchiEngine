
#pragma once

#include <vector>

namespace LitchiRuntime
{
	/**
	* Wraps OpenGL EBO
	*/
	class Framebuffer4Depth
	{
	public:
		/**
		* Create the framebuffer
		* @param p_width
		* @param p_height
		*/
		Framebuffer4Depth(uint16_t p_width = 0, uint16_t p_height = 0);

		/**
		* Destructor
		*/
		~Framebuffer4Depth();

		/**
		* Bind the framebuffer
		*/
		void Bind();

		/**
		* Unbind the framebuffer
		*/
		void Unbind();

		/**
		* Defines a new size for the framebuffer
		* @param p_width
		* @param p_height
		*/
		void Resize(uint16_t p_width, uint16_t p_height);

		/**
		* Returns the ID of the OpenGL framebuffer
		*/
		uint32_t GetID();

		/**
		* Returns the ID of the OpenGL render texture
		*/
		uint32_t GetTextureID();
	private:
		uint32_t m_bufferID = 0;
		uint32_t m_renderTexture = 0;
	};
}