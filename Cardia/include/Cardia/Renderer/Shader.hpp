#pragma once
#include <glm/glm.hpp>

namespace Cardia
{
	class Shader
	{
	public:
		virtual void bind() const {};
		virtual void unbind() const {};

		virtual void setUniformMat4(const std::string& name, glm::mat4 matrix) {};

		static Shader* create(const std::string& vertexSource, const std::string& fragmentSource);
	};
}