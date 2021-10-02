#include "cdpch.hpp"
#include "Platform/Windows/WindowsWin.hpp"
#include "Platform/OpenGL/OpenGLContext.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Cardia/Application.hpp"

namespace Cardia
{
	bool WindowsWin::isGlfwInit = false;

	std::unique_ptr<Window> Window::Create(const WinProperties& properties)
	{
		return std::make_unique<WindowsWin>(properties);
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
			cdAssert(glfwInit(), "Can't initialize glfw3.");
			isGlfwInit = true;
		}
		m_Window = glfwCreateWindow(properties.width,
			properties.height,
			properties.title.c_str(),
			nullptr, nullptr);

		m_RendererContext = std::make_unique<OpenGLContext>(m_Window);		// Just change here to change API Context
		m_RendererContext->init();

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
				RenderCommand::setViewPort(0, 0, w, h);
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

		glfwSetCharCallback(m_Window, [](GLFWwindow* win, uint32_t keycode)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				KeyTypedEvent event(static_cast<int>(keycode));
				data->eventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* win, int button, int action, [[maybe_unused]] int mods)
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
		m_RendererContext->swapBuffers();
	}

	static std::pair<int, int> WinInitPos(GLFWwindow* win)
	{
		int x, y;
		glfwGetWindowPos(win, &x, &y);
		return { x, y };
	}

	void WindowsWin::setFullscreen(bool state)
	{
		static auto winPos = WinInitPos(m_Window);
		static auto winSize = getSize();

		if (state)
		{
			const auto monitor = glfwGetPrimaryMonitor();
			const auto mode = glfwGetVideoMode(monitor);
			glfwGetWindowPos(m_Window, &winPos.first, &winPos.second);
			glfwGetWindowSize(m_Window, &winSize.first, &winSize.second);
			glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, isVSync() ? mode->refreshRate : 0);
		}
		else
		{
			glfwSetWindowMonitor(m_Window, nullptr, winPos.first, winPos.second, winSize.first, winSize.second, 0);
		}

		int vpWidth, vpHeight;
		glfwGetFramebufferSize(m_Window, &vpWidth, &vpHeight);
		RenderCommand::setViewPort(0, 0, vpWidth, vpHeight);
	}

	bool WindowsWin::isFullscreen() const
	{
		return glfwGetWindowMonitor(m_Window) != nullptr;
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
