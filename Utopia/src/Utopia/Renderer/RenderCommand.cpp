#include "utpch.hpp"
#include "RenderCommand.hpp"

#include "Platform/OpenGL/OpenGLRenderAPI.hpp"


namespace Utopia
{
	std::unique_ptr<RenderAPI> RenderCommand::s_RenderAPI = std::make_unique<OpenGLRenderAPI>();
}
