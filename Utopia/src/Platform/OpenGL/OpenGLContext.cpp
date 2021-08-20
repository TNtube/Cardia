#include "utpch.hpp"

#include "Utopia/Core.hpp"
#include "OpenGLContext.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


void Utopia::OpenGLContext::init()
{
	glfwMakeContextCurrent(m_Window);
	const int result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	utCoreAssert(result, "Could not load Glad");
}

void Utopia::OpenGLContext::swapBuffers()
{
	glfwSwapBuffers(m_Window);
}

Utopia::OpenGLContext::OpenGLContext(GLFWwindow *window)
		: m_Window(window)
{
	utCoreAssert(window, "Window is null, please before instantiate a window object !");
}

