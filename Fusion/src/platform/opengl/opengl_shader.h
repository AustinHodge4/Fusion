#pragma once

#include "fusion/renderer/shader.h"

namespace Fusion {

	class OpenGLShader : public Shader 
	{
	public:
		OpenGLShader(const std::string& p_name, const std::string& p_vertexSource, const std::string& p_fragmentSource);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual inline const std::string& GetName() const override { return _name; }
	private:
		uint32_t _rendererID;
		std::string _name;
	};
}