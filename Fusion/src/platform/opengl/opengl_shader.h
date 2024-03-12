#pragma once

#include "fusion/renderer/shader.h"

namespace fusion {

	class OpenGLShader : public Shader 
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_src);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual const std::string get_name() const override { return _name; }
	private:
		uint32_t _renderer_id;
		std::string _name;
	};
}