#pragma once

#include "Utopia/Window.hpp"
#include <GLFW/glfw3.h>
#include "Utopia/Renderer/RendererContext.hpp"


namespace Utopia
{
	struct WinData
	{
		std::string title;
		int width, height;
		bool vSync;

		std::function<void(Event&)> eventCallback;
	};

	class UTOPIA_API WindowsWin : public Window
	{
	public:
		WindowsWin(const WinProperties& properties);
		virtual ~WindowsWin();

		void onUpdate() override;
		inline int getWidth() const override { return m_Data.width; }
		inline int getHeight() const override { return m_Data.height; }
		inline std::pair<int, int> getSize() const override { return {getWidth(), getHeight()}; }


		inline void setEventCallback(const std::function<void(Event&)>& callback) override { m_Data.eventCallback = callback; }
		void setFullscreen(bool state) override;
		bool isFullscreen() const override;
		void setVSync(bool state) override;
		bool isVSync() const override;

		inline virtual void* getNativeWin() const override { return m_Window; }

	private:
		void init(const WinProperties& properties);
		void quit();
		
		GLFWwindow* m_Window;
		WinData m_Data;
		std::unique_ptr<RendererContext> m_RendererContext;
	};
}
