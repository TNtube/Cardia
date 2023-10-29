// Temporary or not, I don't know how I'll approach the linux implementation
#if defined(_WIN64) || defined(__linux__)

#include "cdpch.hpp"
#include "Cardia/Core/Windows/WindowsWin.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

#include <stb_image/stb_image.h>


namespace Cardia
{
	bool WindowsWin::s_isGlfwInit = false;

	std::unique_ptr<Window> Window::Create(const WinProperties& properties)
	{
		return std::make_unique<WindowsWin>(properties);
	}

	WindowsWin::WindowsWin(const WinProperties& properties)
	{
		Init(properties);
	}

	WindowsWin::~WindowsWin()
	{
		Quit();
	}


	void WindowsWin::Init(const WinProperties& properties)
	{
		m_Data.Title = properties.Title;
		m_Data.Width = properties.Width;
		m_Data.Height = properties.Height;

		if (!s_isGlfwInit)
		{
			CdAssert(glfwInit(), "Can't initialize glfw3.");
			s_isGlfwInit = true;
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(properties.Width,
			properties.Height,
			properties.Title.c_str(),
			nullptr, nullptr);

#ifndef __unix__
		{
			int texWidth {}, texHeight {}, texChannels {};
			stbi_uc* pixels = stbi_load(properties.IconPath.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

			const GLFWimage image = { texWidth, texHeight, pixels };
			glfwSetWindowIcon(m_Window, 1, &image);

			stbi_image_free(pixels);
		}
#endif


		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);


		// --------------------------------------------- Glfw callbacks ---------------------------------------------

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* win)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				WindowCloseEvent event;
				data->EventCallback(event);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* win, int w, int h)
			{
				const auto data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				data->Width = w;
				data->Height = h;
				data->ShouldInvalidateSwapchain = true;
				WindowResizeEvent event(w, h);
				data->EventCallback(event);
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* win, int x, int y)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				WindowMoveEvent event(x, y);
				data->EventCallback(event);
			});
		
		glfwSetKeyCallback(m_Window, [](GLFWwindow* win, int key, int scancode, int action, int mods)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				switch (action)
				{
				case GLFW_PRESS:
					{
						KeyDownEvent event(key, 0);
						data->EventCallback(event);
						break;
					}
				case GLFW_RELEASE:
					{
						KeyUpEvent event(key);
						data->EventCallback(event);
						break;
					}
				case GLFW_REPEAT:
					{
						KeyDownEvent event(key, 1);
						data->EventCallback(event);
						break;
					}
				default:
					Log::CoreError("Unknown glfw key callback action");
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* win, uint32_t keycode)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				KeyTypedEvent event(static_cast<int>(keycode));
				data->EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* win, int button, int action, [[maybe_unused]] int mods)
		{
			const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonDownEvent event(button);
					data->EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonUpEvent event(button);
					data->EventCallback(event);
					break;
				}
				default:
					Log::CoreError("Unknown glfw mouse callback action");
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* win, double xOffset, double yOffset)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
				data->EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* win, double xPos, double yPos)
			{
				const WinData* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
				MouseMotionEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
				data->EventCallback(event);
			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* win, int focused)
		{
			auto* data = static_cast<WinData*>(glfwGetWindowUserPointer(win));
			WindowFocusEvent event(focused);
			data->EventCallback(event);
			data->HasFocus = focused;
		});
	}


	void WindowsWin::Quit()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWin::OnUpdate()
	{
		glfwPollEvents();
	}

	static Vector2i WinInitPos(GLFWwindow* win)
	{
		int x, y;
		glfwGetWindowPos(win, &x, &y);
		return { x, y };
	}

	// This method is not immediate, it will only update the fullscreen mode on the next frame
	void WindowsWin::SetFullscreen(bool state)
	{
		m_Data.ShouldUpdateFullscreenMode = true;
		m_Data.IsFullscreen = state;
	}

	void WindowsWin::UpdateFullscreenMode()
	{
		if (m_Data.ShouldUpdateFullscreenMode)
		{
			static auto winPos = WinInitPos(m_Window);
			static auto winSize = GetSize();

			if (m_Data.IsFullscreen)
			{
				const auto monitor = glfwGetPrimaryMonitor();
				const auto mode = glfwGetVideoMode(monitor);

				// Store windows pos data to restore later
				glfwGetWindowPos(m_Window, &winPos.x, &winPos.y);
				glfwGetWindowSize(m_Window, &winSize.x, &winSize.y);

				glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, IsVSync() ? mode->refreshRate : 0);
			}
			else
			{
				glfwSetWindowMonitor(m_Window, nullptr, winPos.x, winPos.y, winSize.x, winSize.y, 0);
			}

			m_Data.ShouldInvalidateSwapchain = true;
			m_Data.ShouldUpdateFullscreenMode = false;
		}
	}

	bool WindowsWin::IsFullscreen() const
	{
		return m_Data.IsFullscreen;
	}

	void WindowsWin::SetVSync(bool state)
	{
		glfwSwapInterval(state);
		m_Data.VSync = state;
		m_Data.ShouldInvalidateSwapchain = true;
	}

	bool WindowsWin::IsVSync() const
	{
		return m_Data.VSync;
	}

	bool WindowsWin::IsFocused() const
	{
		return m_Data.HasFocus;
	}
}

#endif
