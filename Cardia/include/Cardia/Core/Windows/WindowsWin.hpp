#pragma once

#include "Cardia/Core/Window.hpp"
#include "Cardia/Renderer/RenderContext.hpp"

#include <GLFW/glfw3.h>


namespace Cardia
{
	struct WinData
	{
		std::string title;
		int width, height;
		bool vSync;

		std::function<void(Event&)> eventCallback;
	};

	class WindowsWin : public Window
	{
	public:
		WindowsWin(const WinProperties& properties);
		virtual ~WindowsWin() override;

		void onUpdate() override;
		inline int getWidth() const override { return m_Data.width; }
		inline int getHeight() const override { return m_Data.height; }
		inline std::pair<int, int> getSize() const override { return {getWidth(), getHeight()}; }
		inline VkExtent2D GetExtent() const override { return VkExtent2D {static_cast<unsigned>(m_Data.width), static_cast<unsigned>(m_Data.height)}; }


		inline void setEventCallback(const std::function<void(Event&)>& callback) override { m_Data.eventCallback = callback; }
		void setFullscreen(bool state) override;
		bool isFullscreen() const override;
		void setVSync(bool state) override;
		bool isVSync() const override;

		inline virtual void* getNativeWin() const override { return m_Window; }

	private:
		void init(const WinProperties& properties);
		void quit();
		
		static bool s_isGlfwInit; // Can only initialize glfw once.
		GLFWwindow* m_Window;
		WinData m_Data;
	};
}
