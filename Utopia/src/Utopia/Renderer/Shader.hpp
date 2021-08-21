#pragma once

namespace Utopia
{
	class Shader
	{
	public:
		Shader() = default;
		Shader(const std::string& vertexSource, const std::string& fragmentSource);
		void bind() const;
		void unbind() const;

	private:
		unsigned m_ShaderID;
	};
}