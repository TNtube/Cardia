#include "cdpch.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Core/Core.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Cardia
{
	static std::string LoadShader(const std::string& filePath)
	{
		// Code inspired from https://github.com/Razakhel/RaZ/blob/master/src/RaZ/Render/Shader.cpp#L25
		Log::coreInfo("Loading shader (path: '" + filePath + "')...");

		std::ifstream shaderSource(filePath, std::ios::in | std::ios::binary | std::ios::ate);

		const auto fileSize = static_cast<std::size_t>(shaderSource.tellg());
		shaderSource.seekg(0, std::ios::beg);

		std::vector<char> bytes(fileSize);
		shaderSource.read(bytes.data(), static_cast<std::streamsize>(fileSize));

		return bytes.data();
	}

	OpenGLShader::OpenGLShader(const std::string& vertexFilePath, const std::string& fragmentFilePath)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		std::string strSource = LoadShader(vertexFilePath);
		const GLchar* source = strSource.c_str();
		glShaderSource(vertexShader, 1, &source, nullptr);

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

			Log::coreError("Vertex Shader error : {0}", infoLog.data());
			cdCoreAssert(false, "Unable to create OpenGL Vertex Shader !");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		strSource = LoadShader(fragmentFilePath);
		source = strSource.c_str();
		glShaderSource(fragmentShader, 1, &source, nullptr);

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

			Log::coreError("Fragment Shader error : {0}", infoLog.data());
			cdCoreAssert(false, "Unable to create OpenGL Fragment Shader !");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_ShaderID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_ShaderID, vertexShader);
		glAttachShader(m_ShaderID, fragmentShader);

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
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			Log::coreError("Linking Shader error : {0}", infoLog.data());
			cdCoreAssert(false, "Unable to link OpenGL Shaders !");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_ShaderID, vertexShader);
		glDetachShader(m_ShaderID, fragmentShader);
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
}
