#include "fepch.h"

#include "renderer.h"

namespace Fusion {

	void Renderer::BeginScene()
	{

	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& p_vertexArray)
	{
		p_vertexArray->Bind();
		RenderCommand::DrawIndexed(p_vertexArray);
	}
}