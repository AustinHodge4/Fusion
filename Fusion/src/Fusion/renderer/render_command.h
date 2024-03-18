#pragma once

#include "renderer_api.h"

namespace Fusion {

	class RenderCommand 
	{
	public:
		inline static void SetClearColor(const glm::vec4& p_color)
		{
			_rendererAPI->SetClearColor(p_color);
		}

		inline static void Clear()
		{
			_rendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& p_vertexArray)
		{
			_rendererAPI->DrawIndexed(p_vertexArray);
		}
	private:
		static RendererAPI* _rendererAPI;
	};
}