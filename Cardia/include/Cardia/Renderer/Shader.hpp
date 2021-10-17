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
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void setInt(const std::string& name, int value) = 0;

		static std::unique_ptr<Shader> create(std::initializer_list<std::string> filePaths);
	};

	class ShaderManager
	{
	public:
		void add(const std::string& name, std::unique_ptr<Shader> shader);
		Shader* load(const std::string& name, const std::initializer_list<std::string>& filePaths);
		Shader* get(const std::string& name);


	private:
		std::unordered_map<std::string, std::unique_ptr<Shader>> m_Shaders;
	};
}