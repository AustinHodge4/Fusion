#include "fepch.h"
#include "opengl_buffer.h"

#include <glad/glad.h>

namespace fusion {

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) : _renderer_id(0)
	{
		glCreateBuffers(1, &_renderer_id);
		glNamedBufferStorage(_renderer_id, size, vertices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &_renderer_id);
	}

	void OpenGLVertexBuffer::bind(uint32_t vao) const
	{
		uint32_t index = 0;
		for (const auto& element : _layout)
		{
			glVertexArrayVertexBuffer(vao, index, _renderer_id, 0, _layout.get_stride());
			index++;
		}
	}

	void OpenGLVertexBuffer::unbind(uint32_t vao) const
	{
		glVertexArrayVertexBuffer(vao, 0, 0, 0, 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: _renderer_id(0), _count(count)
	{
		glCreateBuffers(1, &_renderer_id);
		glNamedBufferStorage(_renderer_id, count * sizeof(uint32_t), indices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &_renderer_id);
	}

	void OpenGLIndexBuffer::bind(uint32_t vao) const
	{
		glVertexArrayElementBuffer(vao, _renderer_id);
	}

	void OpenGLIndexBuffer::unbind(uint32_t vao) const
	{
		glVertexArrayElementBuffer(vao, 0);
	}
}