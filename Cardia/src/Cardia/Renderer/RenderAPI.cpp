#include "cdpch.hpp"
#include "Cardia/Renderer/RenderAPI.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLRenderAPI.hpp"


namespace Cardia
{
	RenderAPI::API RenderAPI::s_API = API::OpenGL;

	void RenderAPI::init()
	{
		switch (s_API)
		{
		case API::None:
			Log::coreError("{0} is not supported for the moment !", s_API);
			cdCoreAssert(false, "Invalid API provided");
			break;
		case API::OpenGL:
			s_Instance = std::make_unique<OpenGLRenderAPI>();
			break;
		default:
			Log::coreError("{0} is not supported for the moment !", s_API);
			cdCoreAssert(false, "Invalid API provided");
			break;
		}
	}

}
