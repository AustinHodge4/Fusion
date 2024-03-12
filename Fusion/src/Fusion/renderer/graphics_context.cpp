#include "fepch.h"

#include "graphics_context.h"
#include "renderer.h"

#include "platform/opengl/opengl_context.h"

namespace fusion
{
	GraphicsContext* GraphicsContext::create(void* window)
	{
		switch (Renderer::get())
		{
		case RendererAPI::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
		case RendererAPI::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
		case RendererAPI::OpenGL: return new OpenGLContext(static_cast<GLFWwindow*>(window));
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}
}