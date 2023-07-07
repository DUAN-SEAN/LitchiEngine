
#pragma once

#include <vector>

namespace LitchiRuntime
{
	/**
	* Wraps OpenGL VBO
	*/
	template <class T>
	class VertexBuffer
	{
	public:
		/**
		* Create the VBO using a pointer to the first element and a size (number of elements)
		* @param p_data
		* @parma p_elements
		*/
		VertexBuffer(T* p_data, size_t p_elements, size_t elementSize = sizeof(T));

		/**
		* Create the EBO using a vector
		* @param p_data
		*/
		VertexBuffer(std::vector<T>& p_data,size_t elementSize = sizeof(T));

		/**
		* Destructor
		*/
		~VertexBuffer();

		/**
		* Bind the buffer
		*/
		void Bind();

		/**
		* Bind the buffer
		*/
		void Unbind();

		/**
		* Returnd the ID of the VBO
		*/
		uint32_t GetID();

	private:
		uint32_t m_bufferID;
	};
}

#include "VertexBuffer.inl"