#include "fepch.h"
#include "render_command.h"

#include "platform/opengl/opengl_renderer_api.h"

namespace Fusion {

	RendererAPI* RenderCommand::_rendererAPI = new OpenGLRendererAPI();
}