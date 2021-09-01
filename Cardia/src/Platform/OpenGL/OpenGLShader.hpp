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
	private:
		uint32_t m_ShaderID;
	};
}