#include "utpch.hpp"
#include "Shader.hpp"
#include "Utopia/Log.hpp"
#include "Renderer.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Utopia
{
	Shader* Shader::create(const std::string& vertexSource, const std::string& fragmentSource)
	{
		RenderAPI::API renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return new OpenGLShader(vertexSource, fragmentSource);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}
