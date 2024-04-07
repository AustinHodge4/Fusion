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

		virtual void SetInt(const std::string& p_name, int value) override;

		virtual void SetFloat(const std::string& p_name, float value) override;
		virtual void SetFloat2(const std::string& p_name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& p_name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& p_name, const glm::vec4& value) override;

		virtual void SetMat4(const std::string& p_name, const glm::mat4& value) override;
	private:
		uint32_t _rendererID;
		std::string _name;
	};
}