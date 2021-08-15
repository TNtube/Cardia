#include "utpch.hpp"
#include "WindowsWin.hpp"
#include <glad/glad.h>


namespace Utopia
{
	static bool isGlfwInit = false;					// Can only initialize glfw once.

	Window* Window::Create(const WinProperties& properties)
	{
		return new WindowsWin(properties);
	}

	WindowsWin::WindowsWin(const WinProperties& properties)
	{
		init(properties);
	}

	WindowsWin::~WindowsWin()
	{
		quit();
	}


	void WindowsWin::init(const WinProperties& properties)
	{
		m_Data.title = properties.title;
		m_Data.width = properties.width;
		m_Data.height = properties.height;

		if (!isGlfwInit)
		{
			const int result = glfwInit();
			utAssert(result, "Can't initialize glfw3.");

			isGlfwInit = true;
		}
		m_Window = glfwCreateWindow(static_cast<int>(properties.width), 
			static_cast<int>(properties.height), 
			properties.title.c_str(), 
			nullptr, nullptr);

		glfwMakeContextCurrent(m_Window);
		const int result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		utCoreAssert(result, "Could not load Glad");
		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(true);


		// --------------------------------------------- Glfw callbacks ---------------------------------------------

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* win)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				WinCloseEvent event;
				data->eventCallback(event);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* win, int w, int h)
			{
				const auto data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				data->width = w;
				data->height = h;
				WinResizeEvent event(w, h);
				data->eventCallback(event);
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* win, int x, int y)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				WinMoveEvent event(x, y);
				data->eventCallback(event);			
			});
		
		glfwSetKeyCallback(m_Window, [](GLFWwindow* win, int key, int scancode, int action, int mods)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				switch (action)
				{
				case GLFW_PRESS:
					{
						KeyDownEvent event(key, 0);
						data->eventCallback(event);
						break;
					}
				case GLFW_RELEASE:
					{
						KeyUpEvent event(key);
						data->eventCallback(event);
						break;
					}
				case GLFW_REPEAT:
					{
						KeyDownEvent event(key, 1);
						data->eventCallback(event);
						break;
					}
				default:
					Log::coreError("Unknown glfw key callback action");
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* win, unsigned keycode)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				KeyTypedEvent event(static_cast<int>(keycode));
				data->eventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* win, int button, int action, int mods)
		{
			const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonDownEvent event(button);
					data->eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonUpEvent event(button);
					data->eventCallback(event);
					break;
				}
				default:
					Log::coreError("Unknown glfw mouse callback action");
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* win, double xOffset, double yOffset)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
				data->eventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* win, double xPos, double yPos)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				MouseMotionEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
				data->eventCallback(event);
			});
	}


	void WindowsWin::quit()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWin::onUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWin::setVSync(bool state)
	{
		glfwSwapInterval(static_cast<int>(state));		// casting the bool give us the actual value wanted
		m_Data.vSync = state;
	}

	
	bool WindowsWin::isVSync() const
	{
		return m_Data.vSync;
	}
}
