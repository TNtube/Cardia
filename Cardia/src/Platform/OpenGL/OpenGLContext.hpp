#pragma once

#include "Cardia/Renderer/RendererContext.hpp"


struct GLFWwindow;

namespace Cardia {
	class OpenGLContext : public RendererContext
	{
	public:
		explicit OpenGLContext(GLFWwindow* window);

		void init() override;
		void swapBuffers() override;

	private:
		GLFWwindow* m_Window;
	};
}