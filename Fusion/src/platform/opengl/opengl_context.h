#pragma once

#include "fusion/renderer/graphics_context.h"

struct GLFWwindow;

namespace Fusion
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* p_handle);

		virtual void Initialize() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* _handle;
	};
}