#pragma once

#include "Cardia/Renderer/RenderContext.hpp"


struct GLFWwindow;

namespace Cardia {
	class OpenGLContext : public RenderContext
	{
	public:
		explicit OpenGLContext(GLFWwindow* window);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_Window;
	};
}