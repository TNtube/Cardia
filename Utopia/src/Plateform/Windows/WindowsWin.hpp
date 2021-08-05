#pragma once


#include "Utopia/Window.hpp"
#include <GLFW/glfw3.h>


namespace Utopia
{
	struct WinData
	{
		std::string title;
		unsigned width, height;
		bool vSync;

		std::function<void(Event&)> eventCallback;
	};

	class UTOPIA_API WindowsWin : public Window
	{
	public:
		WindowsWin(const WinProperties& properties);
		virtual ~WindowsWin();

		void onUpdate() override;
		inline unsigned getWidth() const override { return m_Data.width; }
		inline unsigned getHeight() const override { return m_Data.height; }
		inline std::pair<unsigned, unsigned> getSize() const override { return {getWidth(), getHeight()}; }


		inline void setEventCallback(const std::function<void(Event&)>& callback) override { m_Data.eventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

	private:
		virtual void init(const WinProperties& properties);
		virtual void quit();
		
		GLFWwindow* m_Window;
		WinData m_Data;
	};
}
