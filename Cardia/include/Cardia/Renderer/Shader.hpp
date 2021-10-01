#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Cardia
{
	class Shader
	{
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Shader* create(const std::string& vertexSource, const std::string& fragmentSource);
	};
}