#include "fepch.h"
#include "opengl_shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Fusion {

	OpenGLShader::OpenGLShader(const std::string& p_name, const std::string& p_vertexSource, const std::string& p_fragmentSource) 
		: _rendererID(0), _name(p_name) {
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = p_vertexSource.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			FE_CORE_ERROR("{0}", infoLog.data());
			FE_CORE_ASSERT(false, "Vertex shader compile failure!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = p_fragmentSource.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			FE_CORE_ERROR("{0}", infoLog.data());
			FE_CORE_ASSERT(false, "Fragment shader compile failure!");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		_rendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(_rendererID, vertexShader);
		glAttachShader(_rendererID, fragmentShader);

		// Link our program
		glLinkProgram(_rendererID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(_rendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(_rendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(_rendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			FE_CORE_ERROR("{0}", infoLog.data());
			FE_CORE_ASSERT(false, "Linking shader failure!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(_rendererID, vertexShader);
		glDetachShader(_rendererID, fragmentShader);
	}

	OpenGLShader::~OpenGLShader() 
	{
		glDeleteProgram(_rendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(_rendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& p_name, int value)
	{
		int32_t uniformID = glGetUniformLocation(_rendererID, p_name.c_str());
		glUniform1i(uniformID, value);
	}

	void OpenGLShader::SetFloat(const std::string& p_name, float value)
	{
		int32_t uniformID = glGetUniformLocation(_rendererID, p_name.c_str());
		glUniform1f(uniformID, value);
	}

	void OpenGLShader::SetFloat2(const std::string& p_name, const glm::vec2& value)
	{
		int32_t uniformID = glGetUniformLocation(_rendererID, p_name.c_str());
		glUniform2f(uniformID, value.x, value.y);
	}

	void OpenGLShader::SetFloat3(const std::string& p_name, const glm::vec3& value)
	{
		int32_t uniformID = glGetUniformLocation(_rendererID, p_name.c_str());
		glUniform3f(uniformID, value.x, value.y, value.z);
	}

	void OpenGLShader::SetFloat4(const std::string& p_name, const glm::vec4& value)
	{
		int32_t uniformID = glGetUniformLocation(_rendererID, p_name.c_str());
		glUniform4f(uniformID, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetMat4(const std::string& p_name, const glm::mat4& value)
	{
		uint32_t uniformID = glGetUniformLocation(_rendererID, p_name.c_str());
		glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(value));
	}
}