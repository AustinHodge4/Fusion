#include "fepch.h"
#include "vertex_array.h"

#include "renderer.h"
#include "platform/opengl/opengl_vertex_array.h"

namespace fusion {

	VertexArray* fusion::VertexArray::create()
	{
		switch (Renderer::get())
		{
			case RendererAPI::API::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
			case RendererAPI::API::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}
}