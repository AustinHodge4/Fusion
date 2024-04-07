#pragma once

#include "render_command.h"

#include "camera.h"
#include "shader.h"

namespace Fusion {

	class Renderer 
	{
	public:
		static void BeginScene(OrthographicCamera& p_camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader> p_shader, const std::shared_ptr<VertexArray>& p_vertexArray, const glm::mat4& transform);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* _sceneData;
	};
}