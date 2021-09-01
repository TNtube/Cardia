#include "cdpch.hpp"
#include "Shader.hpp"
#include "Cardia/Log.hpp"
#include "Renderer.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Cardia
{
	Shader* Shader::create(const std::string& vertexSource, const std::string& fragmentSource)
	{
		RenderAPI::API renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return new OpenGLShader(vertexSource, fragmentSource);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}
