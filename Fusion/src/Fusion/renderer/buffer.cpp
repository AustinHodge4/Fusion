#include "fepch.h"
#include "buffer.h"

#include "renderer.h"

#include "platform/opengl/opengl_buffer.h"

namespace Fusion {

	Ref<VertexBuffer> VertexBuffer::Create(float* p_vertices, uint32_t p_size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
			case RendererAPI::API::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(p_vertices, p_size);
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* p_indices, uint32_t p_size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
		case RendererAPI::API::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef <OpenGLIndexBuffer>(p_indices, p_size);
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}
}