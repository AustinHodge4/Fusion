#include "fepch.h"
#include "opengl_buffer.h"

#include <glad/glad.h>

namespace Fusion {

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* p_vertices, uint32_t p_size) : _rendererID(0)
	{
		glCreateBuffers(1, &_rendererID);
		glNamedBufferStorage(_rendererID, p_size, p_vertices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &_rendererID);
	}

	void OpenGLVertexBuffer::Bind(uint32_t p_vertexArray) const
	{
		uint32_t index = 0;
		for (const auto& element : _layout)
		{
			glVertexArrayVertexBuffer(p_vertexArray, index, _rendererID, 0, _layout.GetStride());
			index++;
		}
	}

	void OpenGLVertexBuffer::Unbind(uint32_t p_vertexArray) const
	{
		glVertexArrayVertexBuffer(p_vertexArray, 0, 0, 0, 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* p_indices, uint32_t p_count)
		: _rendererID(0), _count(p_count)
	{
		glCreateBuffers(1, &_rendererID);
		glNamedBufferStorage(_rendererID, p_count * sizeof(uint32_t), p_indices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &_rendererID);
	}

	void OpenGLIndexBuffer::Bind(uint32_t p_vertexArray) const
	{
		glVertexArrayElementBuffer(p_vertexArray, _rendererID);
	}

	void OpenGLIndexBuffer::Unbind(uint32_t p_vertexArray) const
	{
		glVertexArrayElementBuffer(p_vertexArray, 0);
	}
}