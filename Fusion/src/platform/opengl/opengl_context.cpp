#include "fepch.h"
#include "opengl_context.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Fusion
{
	void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		const char* source_, * type_, * severity_;

		switch (source)
		{
			case GL_DEBUG_SOURCE_API:             source_ = "API";             break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_ = "WINDOW_SYSTEM";   break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: source_ = "SHADER_COMPILER"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     source_ = "THIRD_PARTY";     break;
			case GL_DEBUG_SOURCE_APPLICATION:     source_ = "APPLICATION";     break;
			case GL_DEBUG_SOURCE_OTHER:           source_ = "OTHER";           break;
			default:                              source_ = "<SOURCE>";        break;
		}

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:               type_ = "ERROR";               break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_ = "DEPRECATED_BEHAVIOR"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_ = "UDEFINED_BEHAVIOR";   break;
			case GL_DEBUG_TYPE_PORTABILITY:         type_ = "PORTABILITY";         break;
			case GL_DEBUG_TYPE_PERFORMANCE:         type_ = "PERFORMANCE";         break;
			case GL_DEBUG_TYPE_OTHER:               type_ = "OTHER";               break;
			case GL_DEBUG_TYPE_MARKER:              type_ = "MARKER";              break;
			default:                                type_ = "<TYPE>";              break;
		}

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         severity_ = "HIGH";         break;
			case GL_DEBUG_SEVERITY_MEDIUM:       severity_ = "MEDIUM";       break;
			case GL_DEBUG_SEVERITY_LOW:          severity_ = "LOW";          break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: severity_ = "NOTIFICATION"; break;
			default:                             severity_ = "<SEVERITY>";   break;
		}

		if (type == GL_DEBUG_TYPE_ERROR) {
			FE_CORE_ERROR("[OpenGL] {0} ({1}) - {2}", type_, source_, message);
		} else {
			FE_CORE_INFO("[OpenGL] {0} ({1}) - {2}", type_, source_, message);
		}
	}

	OpenGLContext::OpenGLContext(GLFWwindow* p_handle) : _handle(p_handle)
	{
		FE_CORE_ASSERT(_handle, "Windows handle is null!");
	}

	void OpenGLContext::Initialize()
	{
		glfwMakeContextCurrent(_handle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FE_CORE_ASSERT(status, "Failed to init glad!");

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);

		FE_CORE_INFO("OpenGL Info:");
		FE_CORE_INFO("  Vendor: {0}", (const char*) glGetString(GL_VENDOR));
		FE_CORE_INFO("  Renderer: {0}", (const char*) glGetString(GL_RENDERER));
		FE_CORE_INFO("  Version: {0}", (const char*) glGetString(GL_VERSION));

		FE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Fusion requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(_handle);
	}
}