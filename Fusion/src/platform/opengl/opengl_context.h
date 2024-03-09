#pragma once

#include "fusion/renderer/graphics_context.h"

struct GLFWwindow;

namespace fusion
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* handle);

		virtual void init() override;
		virtual void swap_buffers() override;
	private:
		GLFWwindow* _handle;
	};
}