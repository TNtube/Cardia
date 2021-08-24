#pragma once
#include "Utopia/Renderer/Shader.hpp"

namespace Utopia
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