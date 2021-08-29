#include "utpch.hpp"
#include "OpenGLRenderCommand.hpp"
#include <glad/glad.h>

namespace Utopia
{

	void OpenGLRenderCommand::setViewPort(int x, int y, int w, int h) const
	{
		glViewport(x, y, w, h);
	}

	void OpenGLRenderCommand::setWireFrame(bool state) const
	{
		glPolygonMode(GL_FRONT_AND_BACK, state ? GL_LINE : GL_FILL);
	}
}