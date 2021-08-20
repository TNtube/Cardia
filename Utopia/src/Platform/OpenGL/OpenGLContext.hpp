#pragma once

#include "Utopia/Renderer/RendererContext.hpp"


struct GLFWwindow;

namespace Utopia {
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