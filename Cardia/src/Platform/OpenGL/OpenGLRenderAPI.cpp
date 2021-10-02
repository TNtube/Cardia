#include "cdpch.hpp"
#include "Platform/OpenGL/OpenGLRenderAPI.hpp"
#include <glad/glad.h>

namespace Cardia
{
	void OpenGLRenderAPI::setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderAPI::setViewPort(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h);
	}

	void OpenGLRenderAPI::setWireFrame(bool state)
	{
		glPolygonMode(GL_FRONT_AND_BACK, state ? GL_LINE : GL_FILL);
	}

	void OpenGLRenderAPI::drawIndexed(const std::unique_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
	}
}
