
#pragma once

#include "ShaderStorageBuffer.h"

namespace LitchiRuntime
{
	template <class T>
	inline VertexBuffer<T>::VertexBuffer(T* p_data, size_t p_elements, size_t elementSize)
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glBufferData(GL_ARRAY_BUFFER, p_elements * elementSize, p_data, GL_STATIC_DRAW);
	}

	template<class T>
	inline VertexBuffer<T>::VertexBuffer(std::vector<T>& p_data, size_t elementSize) : VertexBuffer(p_data.data(), p_data.size(), elementSize)
	{
	}

	template<class T>
	inline VertexBuffer<T>::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	template <class T>
	inline void VertexBuffer<T>::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	}

	template <class T>
	inline void VertexBuffer<T>::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template <class T>
	inline uint32_t VertexBuffer<T>::GetID()
	{
		return m_bufferID;
	}
}
