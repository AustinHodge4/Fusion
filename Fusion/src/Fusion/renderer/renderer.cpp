#include "fepch.h"
#include "renderer.h"

namespace Fusion {

	Renderer::SceneData* Renderer::_sceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthographicCamera& p_camera)
	{
		_sceneData->viewProjectionMatrix = p_camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<Shader> p_shader, const std::shared_ptr<VertexArray>& p_vertexArray, const glm::mat4& p_transform = glm::mat4(1))
	{
		p_shader->Bind();
		p_shader->SetMat4("u_viewProjection", _sceneData->viewProjectionMatrix);
		p_shader->SetMat4("u_transform", p_transform);

		p_vertexArray->Bind();
		RenderCommand::DrawIndexed(p_vertexArray);
	}
}