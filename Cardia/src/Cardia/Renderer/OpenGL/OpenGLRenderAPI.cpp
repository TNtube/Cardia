#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLRenderAPI.hpp"
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

	void OpenGLRenderAPI::drawIndexed(const VertexArray* vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->getIndexBuffer().getCount();
		glDrawElements(GL_TRIANGLES, static_cast<int>(count), GL_UNSIGNED_INT, nullptr);

	}

	std::string OpenGLRenderAPI::getVendor()
	{
		return {reinterpret_cast<const char*>(glGetString(GL_VENDOR))};
	}

	std::string OpenGLRenderAPI::getRenderer()
	{
		return {reinterpret_cast<const char*>(glGetString(GL_RENDERER))};
	}

	std::string OpenGLRenderAPI::getVersion()
	{
		return {reinterpret_cast<const char*>(glGetString(GL_VERSION))};
	}
}
