#pragma once

namespace fusion {

	enum class RendererAPI
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
	};
	class Renderer 
	{
	public:
		inline static RendererAPI get() { return _api; }
	private:
		static RendererAPI _api;
	};
}