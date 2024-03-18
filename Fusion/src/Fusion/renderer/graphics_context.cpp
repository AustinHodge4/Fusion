#include "fepch.h"

#include "graphics_context.h"
#include "renderer.h"

#include "platform/opengl/opengl_context.h"

namespace Fusion
{
	GraphicsContext* GraphicsContext::Create(void* p_window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
		case RendererAPI::API::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLContext(static_cast<GLFWwindow*>(p_window));
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}
}