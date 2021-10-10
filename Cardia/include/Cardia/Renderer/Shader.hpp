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

		static std::shared_ptr<Shader> create(std::initializer_list<std::string> filePaths);
	};

	class ShaderManager
	{
	public:
		void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
		std::shared_ptr<Shader> load(const std::string& name, const std::initializer_list<std::string>& filePaths);
		std::shared_ptr<Shader> get(const std::string& name);

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}