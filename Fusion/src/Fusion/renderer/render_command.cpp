#include "fepch.h"
#include "render_command.h"

#include "platform/opengl/opengl_renderer_api.h"

namespace Fusion {
	// TODO: Choose API dynamically
	RendererAPI* RenderCommand::_rendererAPI = new OpenGLRendererAPI();
}