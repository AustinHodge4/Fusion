#include "fepch.h"
#include "opengl_vertex_array.h"

#include <glad/glad.h>

namespace Fusion {

	static GLenum ShaderDataTypeToGL(ShaderDataType p_type)
	{
		switch (p_type)
		{
			case Fusion::ShaderDataType::Float:		return GL_FLOAT;
			case Fusion::ShaderDataType::Float2:	return GL_FLOAT;
			case Fusion::ShaderDataType::Float3:	return GL_FLOAT;
			case Fusion::ShaderDataType::Float4:	return GL_FLOAT;
			case Fusion::ShaderDataType::Mat3:		return GL_FLOAT;
			case Fusion::ShaderDataType::Mat4:		return GL_FLOAT;
			case Fusion::ShaderDataType::Int:		return GL_INT;
			case Fusion::ShaderDataType::Int2:		return GL_INT;
			case Fusion::ShaderDataType::Int3:		return GL_INT;
			case Fusion::ShaderDataType::Int4:		return GL_INT;
			case Fusion::ShaderDataType::Bool:		return GL_BOOL;
		}
		FE_CORE_ASSERT(false, "Unknown shader type");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() 
	{
		glCreateVertexArrays(1, &_rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDisableVertexArrayAttrib(_rendererID, 0);
		glDeleteVertexArrays(1, &_rendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(_rendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& p_vertexBuffer)
	{
		FE_CORE_ASSERT(p_vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

		p_vertexBuffer->Bind(_rendererID);
		
		uint32_t index = 0;
		const auto& layout = p_vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glVertexArrayAttribFormat(_rendererID, index, element.GetComponentCount(), ShaderDataTypeToGL(element.type), element.normalized, element.offset);
			glVertexArrayAttribBinding(_rendererID, index, 0);
			glEnableVertexArrayAttrib(_rendererID, index);

			index++;
		}

		_vertexBuffers.push_back(p_vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& p_indexBuffer)
	{
		p_indexBuffer->Bind(_rendererID);
		_indexBuffer = p_indexBuffer;
	}
}