#pragma once

#include "Cardia/Renderer/Shader.hpp"

#include <initializer_list>


namespace Cardia
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(std::initializer_list<std::string> filePaths);
		void bind() const override;
		void unbind() const override;

		void setBindingBlock(const char* name, uint32_t binding) override;

		void setFloat4(const std::string& name, const glm::vec4& value) override;
		void setFloat3(const std::string& name, const glm::vec3& value) override;
		void setMat3(const std::string& name, const glm::mat3& value) override;
		void setMat4(const std::string& name, const glm::mat4& value) override;
		void setInt(const std::string& name, int value) override;
		void setIntArray(const std::string& name, int* values, int count) override;

		void setUniformMat3(const std::string& name, glm::mat3 matrix) const;
		void setUniformMat4(const std::string& name, glm::mat4 matrix) const;
		void setUniformFloat4(const std::string& name, glm::vec4 data) const;
		void setUniformFloat3(const std::string& name, glm::vec3 data) const;
		void setUniformInt(const std::string& name, int value) const;
		void setUniformIntArray(const std::string& name, int* values, int count) const;
	private:
		friend class OpenGLUniformBuffer;
		uint32_t m_ShaderID;
	};
}