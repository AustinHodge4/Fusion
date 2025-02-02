#include "fepch.h"
#include "renderer.h"

namespace Fusion {

	Renderer::SceneData* Renderer::_sceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& p_camera)
	{
		_sceneData->viewProjectionMatrix = p_camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Ref<Shader> p_shader, const Ref<VertexArray>& p_vertexArray, const glm::mat4& p_transform = glm::mat4(1))
	{
		p_shader->Bind();
		p_shader->SetMat4("u_view_projection", _sceneData->viewProjectionMatrix);
		p_shader->SetMat4("u_transform", p_transform);

		p_vertexArray->Bind();
		RenderCommand::DrawIndexed(p_vertexArray);
	}
}