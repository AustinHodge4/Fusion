#pragma once

#include <glm/glm.hpp>

namespace Fusion {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual const std::string& GetName() const = 0;


		virtual void SetInt(const std::string& p_name, int value) = 0;

		virtual void SetFloat(const std::string& p_name, float value) = 0;
		virtual void SetFloat2(const std::string& p_name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& p_name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& p_name, const glm::vec4& value) = 0;

		virtual void SetMat4(const std::string& p_name, const glm::mat4& value) = 0;

		static Shader* Create(const std::string& p_name, const std::string& p_vertexSource, const std::string& p_fragmentSource);
	};

	class ShaderLibrary 
	{
	public:
		void Add(const std::string& p_name, Shader& p_shader);
		Shader* GetShader(const std::string& p_name) const;

		bool Exists(const std::string& p_name) const;
		
	private:
		std::unordered_map<std::string, Shader*> _library;
	};
}