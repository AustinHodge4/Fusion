#pragma once

#include "fusion/renderer/renderer_api.h"

namespace Fusion {

	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void SetClearColor(const glm::vec4& p_coloe) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& p_vertexArray) override;
	};
}