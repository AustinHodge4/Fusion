#include "fepch.h"
#include "shader.h"

#include "renderer.h"

#include "platform/opengl/opengl_shader.h"

namespace Fusion {

	Ref<Shader> Shader::Create(const std::string& p_name, const std::string& p_vertexSource, const std::string& p_fragmentSource)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
			case RendererAPI::API::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(p_name, p_vertexSource, p_fragmentSource);
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& p_name, Ref<Shader>& p_shader)
	{
		FE_CORE_ASSERT(!Exists(p_shader->GetName()), "Shader already exists!");
		_library[p_name] = p_shader;
	}

	Ref<Shader> ShaderLibrary::GetShader(const std::string& p_name) const
	{
		FE_CORE_ASSERT(Exists(p_name), "Shader does not exists!");
		return _library.at(p_name);
	}

	bool ShaderLibrary::Exists(const std::string& p_name) const
	{
		return _library.find(p_name) != _library.end();
	}
}
