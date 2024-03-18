#pragma once

#include "render_command.h"

namespace fusion {

	class Renderer 
	{
	public:
		static void begin_scene();
		static void end_scene();

		static void submit(const std::shared_ptr<VertexArray>& vertex_array);

		inline static RendererAPI::API get() { return RendererAPI::get_api(); }
	};
}