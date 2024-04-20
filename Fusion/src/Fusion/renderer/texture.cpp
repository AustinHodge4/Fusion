#include "fepch.h"
#include "texture.h"

#include "renderer.h"
#include "platform/opengl/opengl_texture.h"

namespace Fusion {
	Ref<Texture2D> Texture2D::Create(const std::string& p_path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
		case RendererAPI::API::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(p_path);
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}
}