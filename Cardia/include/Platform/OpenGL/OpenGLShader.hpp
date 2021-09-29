#pragma once
#include "Cardia/Renderer/Shader.hpp"

namespace Cardia
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
		void bind() const override;
		void unbind() const override;
		void setUniformMat4(const std::string& name, glm::mat4 matrix) override;
	private:
		uint32_t m_ShaderID;
	};
}