#include "fepch.h"
#include "shader.h"

#include "renderer.h"

#include "platform/opengl/opengl_shader.h"

namespace fusion {

	Shader* Shader::create(const std::string& name, const std::string& vertex_src, const std::string& fragment_src)
	{
		switch (Renderer::get())
		{
			case RendererAPI::None: FE_CORE_ASSERT(false, "Renderer API None not supported!"); return nullptr;
			case RendererAPI::Vulkan: FE_CORE_ASSERT(false, "Renderer API Vulkan not supported!"); return nullptr;
			case RendererAPI::OpenGL: return new OpenGLShader(name, vertex_src, fragment_src);
		}

		FE_CORE_ASSERT(false, "Unknown Renderer API not supported!");
		return nullptr;
	}

	void ShaderLibrary::add(const std::string& name, Shader& shader)
	{
		FE_CORE_ASSERT(!exists(shader.get_name()), "Shader already exists!");
		_library[name] = &shader;
	}

	Shader* ShaderLibrary::get(const std::string& name) const
	{
		FE_CORE_ASSERT(exists(name), "Shader does not exists!");
		return _library.at(name);
	}

	bool ShaderLibrary::exists(const std::string& name) const
	{
		return _library.find(name) != _library.end();
	}
}
