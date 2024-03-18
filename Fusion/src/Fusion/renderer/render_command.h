#pragma once

#include "renderer_api.h"

namespace fusion {

	class RenderCommand 
	{
	public:
		inline static void set_clear_color(const glm::vec4& color)
		{
			_renderer_api->set_clear_color(color);
		}

		inline static void clear()
		{
			_renderer_api->clear();
		}

		inline static void draw_indexed(const std::shared_ptr<VertexArray>& vertex_array)
		{
			_renderer_api->draw_indexed(vertex_array);
		}
	private:
		static RendererAPI* _renderer_api;
	};
}