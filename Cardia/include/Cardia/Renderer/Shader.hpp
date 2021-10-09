#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace Cardia
{
	class Shader
	{
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static std::unique_ptr<Shader> create(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	};
}