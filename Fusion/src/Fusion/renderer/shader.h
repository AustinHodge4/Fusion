#pragma once

namespace fusion {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual const std::string get_name() const = 0;

		static Shader* create(const std::string& name, const std::string& vertex_src, const std::string& fragment_src);
	};

	class ShaderLibrary 
	{
	public:
		void add(const std::string& name, Shader& shader);
		Shader* get(const std::string& name) const;

		bool exists(const std::string& name) const;
		
	private:
		std::unordered_map<std::string, Shader*> _library;
	};
}