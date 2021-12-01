#include "cdpch.hpp"
#include "Cardia/Renderer/RenderCommand.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLRenderAPI.hpp"


namespace Cardia
{
	std::unique_ptr<RenderAPI> RenderCommand::s_RenderAPI = std::make_unique<OpenGLRenderAPI>();
}
