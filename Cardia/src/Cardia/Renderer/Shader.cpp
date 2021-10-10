#include "cdpch.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Cardia
{
	std::shared_ptr<Shader> Shader::create(std::initializer_list<std::string> filePaths)
	{
		RenderAPI::API renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(filePaths);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	void ShaderManager::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		Log::coreTrace("Testing 1");
		m_Shaders.insert({ name, shader });
		Log::coreTrace("Testing 2");
	}

	std::shared_ptr<Shader>
	ShaderManager::load(const std::string &name, const std::initializer_list<std::string>& filePaths)
	{
		std::shared_ptr<Shader> shader = Shader::create(filePaths);
		Add(name, shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderManager::get(const std::string &name)
	{
		return m_Shaders[name];
	}
}
