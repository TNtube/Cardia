#include "cdpch.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Cardia
{
	std::unique_ptr<Shader> Shader::create(std::initializer_list<std::string> filePaths)
	{
		RenderAPI::API renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLShader>(filePaths);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	void ShaderManager::add(const std::string& name, std::unique_ptr<Shader> shader)
	{
		m_Shaders.insert({ name, std::move(shader) });
	}

	Shader* ShaderManager::load(const std::string &name, const std::initializer_list<std::string>& filePaths)
	{
		add(name, Shader::create(filePaths));
		return get(name);
	}

	Shader* ShaderManager::get(const std::string &name)
	{
		return m_Shaders[name].get();
	}
}
