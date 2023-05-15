
#pragma once

#include <vector>

#include "EAccessSpecifier.h"

namespace OvRendering::Resources { class Shader; }

namespace LitchiRuntime
{
	/**
	* Wraps OpenGL SSBO
	*/
	class ShaderStorageBuffer
	{
	public:
		/**
		* Create a SSBO with the given access specifier hint
		*/
		ShaderStorageBuffer(EAccessSpecifier p_accessSpecifier);

		/**
		* Destroy the SSBO
		*/
		~ShaderStorageBuffer();

		/**
		* Bind the SSBO to the given binding point
		* @param p_bindingPoint
		*/
		void Bind(uint32_t p_bindingPoint);

		/**
		* Unbind the SSBO from the currently binding point
		*/
		void Unbind();

		/**
		* Send the block data
		*/
		template<typename T>
		void SendBlocks(T* p_data, size_t p_size);

	private:
		uint32_t m_bufferID;
		uint32_t m_bindingPoint = 0;
	};
}

#include "ShaderStorageBuffer.inl"