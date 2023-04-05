#include "cdpch.hpp"
#include "Cardia/Core/Core.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLContext.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


void Cardia::OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_Window);
	const int result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	cdCoreAssert(result, "Could not load Glad");
	Log::coreInfo("OpenGL : {0} | {1} | {2}",
			glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Cardia::OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_Window);
}

Cardia::OpenGLContext::OpenGLContext(GLFWwindow *window)
	: m_Window(window)
{
	cdCoreAssert(window, "Window is null, please before instantiate a window object !");
}

