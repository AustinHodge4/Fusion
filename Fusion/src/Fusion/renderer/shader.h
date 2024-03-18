#pragma once

namespace Fusion {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual const std::string& GetName() const = 0;

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