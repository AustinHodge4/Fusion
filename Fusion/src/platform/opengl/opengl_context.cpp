#include "fepch.h"

#include "opengl_context.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace fusion
{
	OpenGLContext::OpenGLContext(GLFWwindow* handle) : _handle(handle)
	{
		FE_CORE_ASSERT(_handle, "Windows handle is null!");
	}

	void OpenGLContext::init()
	{
		glfwMakeContextCurrent(_handle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FE_CORE_ASSERT(status, "Failed to init glad!");

		FE_CORE_INFO("OpenGL Info:");
		FE_CORE_INFO("  Vendor: {0}", (const char*) glGetString(GL_VENDOR));
		FE_CORE_INFO("  Renderer: {0}", (const char*) glGetString(GL_RENDERER));
		FE_CORE_INFO("  Version: {0}", (const char*) glGetString(GL_VERSION));

		FE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Fusion requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::swap_buffers()
	{
		glfwSwapBuffers(_handle);
	}
}