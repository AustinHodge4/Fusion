#pragma once

#include "fusion/renderer/renderer_api.h"

namespace fusion {

	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void set_clear_color(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void draw_indexed(const std::shared_ptr<VertexArray>& vertex_array) override;
	};
}