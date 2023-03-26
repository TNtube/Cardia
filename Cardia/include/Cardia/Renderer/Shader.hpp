#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace Cardia
{
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		
		virtual void setBindingBlock(const char* name, uint32_t binding) = 0;

		virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void setMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void setInt(const std::string& name, int value) = 0;
		virtual void setIntArray(const std::string& name, int* values, int count) = 0;

		static std::unique_ptr<Shader> create(std::initializer_list<std::string> filePaths);
	};
}