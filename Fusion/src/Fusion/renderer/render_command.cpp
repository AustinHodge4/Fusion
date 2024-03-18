#include "fepch.h"
#include "render_command.h"

#include "platform/opengl/opengl_renderer_api.h"

namespace fusion {

	RendererAPI* RenderCommand::_renderer_api = new OpenGLRendererAPI();
}