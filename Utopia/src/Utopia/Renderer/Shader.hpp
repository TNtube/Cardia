#pragma once

namespace Utopia
{
	class Shader
	{
	public:
		virtual void bind() const {};
		virtual void unbind() const {};
		static Shader* create(const std::string& vertexSource, const std::string& fragmentSource);
	};
}