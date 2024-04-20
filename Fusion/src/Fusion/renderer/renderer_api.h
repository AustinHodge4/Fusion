#pragma once

#include "vertex_array.h"

#include <glm/glm.hpp>

namespace Fusion {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan = 2
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& p_color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& p_vertexArray) = 0;

		inline static API GetAPI() { return _api; }
	private:
		static API _api;
	};
}