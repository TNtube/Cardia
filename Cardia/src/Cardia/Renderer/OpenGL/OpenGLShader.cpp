#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLShader.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Core/Core.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


namespace Cardia
{
	static std::string LoadShader(const std::string& filePath)
	{
		Log::coreInfo("Loading shader (path: '" + filePath + "')...");
		std::ifstream t(filePath);

		return { std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>() };
	}

	static constexpr inline auto string_hash(const char *s) {
		unsigned long long hash{}, c{};
		for (auto p = s; *p; ++p, ++c) {
			hash += *p << c;
		}
		return hash;
	}

	static constexpr inline auto operator"" _sh(const char *s, size_t) {
		return string_hash(s);
	}

	OpenGLShader::OpenGLShader(std::initializer_list<std::string> filePaths)
	{
		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_ShaderID = glCreateProgram();

		std::vector<GLuint> shaderIDs {};

		for (auto& filePath : filePaths)
		{
			GLenum shaderType;
			std::string extension = std::filesystem::path(filePath).extension().string();
			switch (string_hash(extension.c_str()+1))
			{
				case "frag"_sh:
					shaderType = GL_FRAGMENT_SHADER;
					break;
				case "vert"_sh:
					shaderType = GL_VERTEX_SHADER;
					break;
				default:
					shaderType = GL_NONE;
					Log::coreError(extension);
					cdCoreAssert(false, "Unsupported shader extension !");
					break;
			}

			// Create an empty shader handle
			GLuint shader = glCreateShader(shaderType);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			std::string strSource = LoadShader(filePath);
			const GLchar* source = strSource.c_str();
			glShaderSource(shader, 1, &source, nullptr);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				Log::coreError("Shader error : {0}", infoLog.data());
				cdCoreAssert(false, "Unable to create OpenGL Shader !");
				return;
			}
			shaderIDs.push_back(shader);
		}
		
		for(auto& shaderID : shaderIDs)
		{
			glAttachShader(m_ShaderID, shaderID);
		}

		// Link our program
		glLinkProgram(m_ShaderID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_ShaderID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_ShaderID);

			for (auto& shaderID : shaderIDs)
			{
				// Don't leak shaders either.
				glDeleteShader(shaderID);
			}

			Log::coreError("Linking Shader error : {0}", infoLog.data());
			cdCoreAssert(false, "Unable to link OpenGL Shaders !");
			return;
		}

		for (auto& shaderID : shaderIDs)
		{
			glDetachShader(m_ShaderID, shaderID);
		}
	}

	void OpenGLShader::bind() const
	{
		glUseProgram(m_ShaderID);
	}

	void OpenGLShader::unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::setUniformMat4(const std::string& name, glm::mat4 matrix) const
	{
		GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::setUniformFloat4(const std::string& name, glm::vec4 data) const
	{
		GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform4f(location, data.x, data.y, data.z, data.w);
	}

	void OpenGLShader::setUniformFloat3(const std::string& name, glm::vec3 data) const
	{
		GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform3f(location, data.x, data.y, data.z);
	}

	void OpenGLShader::setUniformInt(const std::string &name, int value) const
	{
		GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::setUniformIntArray(const std::string &name, int *values, int count) const
	{
		GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::setFloat4(const std::string &name, const glm::vec4 &value)
	{
		setUniformFloat4(name, value);
	}

	void OpenGLShader::setMat4(const std::string &name, const glm::mat4 &value)
	{
		setUniformMat4(name, value);
	}

	void OpenGLShader::setInt(const std::string &name, int value)
	{
		setUniformInt(name, value);
	}

	void OpenGLShader::setIntArray(const std::string &name, int *values, int count)
	{
		setUniformIntArray(name, values, count);
	}
}
