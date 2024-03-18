#include "fepch.h"
#include "opengl_renderer_api.h"

#include <glad/glad.h>

namespace Fusion {

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& p_color)
	{
		glClearColor(p_color.r, p_color.g, p_color.b, p_color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& p_vertexArray)
	{
		glDrawElements(GL_TRIANGLES, p_vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}