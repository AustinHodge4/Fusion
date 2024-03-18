#include "fepch.h"
#include "opengl_vertex_array.h"

#include <glad/glad.h>

namespace fusion {

	static GLenum ShaderDataTypeToGL(ShaderDataType type)
	{
		switch (type)
		{
			case fusion::ShaderDataType::Float:		return GL_FLOAT;
			case fusion::ShaderDataType::Float2:	return GL_FLOAT;
			case fusion::ShaderDataType::Float3:	return GL_FLOAT;
			case fusion::ShaderDataType::Float4:	return GL_FLOAT;
			case fusion::ShaderDataType::Mat3:		return GL_FLOAT;
			case fusion::ShaderDataType::Mat4:		return GL_FLOAT;
			case fusion::ShaderDataType::Int:		return GL_INT;
			case fusion::ShaderDataType::Int2:		return GL_INT;
			case fusion::ShaderDataType::Int3:		return GL_INT;
			case fusion::ShaderDataType::Int4:		return GL_INT;
			case fusion::ShaderDataType::Bool:		return GL_BOOL;
		}
		FE_CORE_ASSERT(false, "Unknown shader type");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() 
	{
		glCreateVertexArrays(1, &_renderer_id);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDisableVertexArrayAttrib(_renderer_id, 0);
		glDeleteVertexArrays(1, &_renderer_id);
	}

	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(_renderer_id);
	}

	void OpenGLVertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer)
	{
		FE_CORE_ASSERT(vertex_buffer->get_layout().get_elements().size(), "Vertex buffer has no layout!");

		vertex_buffer->bind(_renderer_id);
		
		uint32_t index = 0;
		const auto& layout = vertex_buffer->get_layout();
		for (const auto& element : layout)
		{
			glVertexArrayAttribFormat(_renderer_id, index, element.GetComponentCount(), ShaderDataTypeToGL(element.type), element.normalized, element.offset);
			glVertexArrayAttribBinding(_renderer_id, index, 0);
			glEnableVertexArrayAttrib(_renderer_id, index);

			index++;
		}

		_vertex_buffers.push_back(vertex_buffer);
	}

	void OpenGLVertexArray::set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer)
	{
		index_buffer->bind(_renderer_id);
		_index_buffer = index_buffer;
	}
}