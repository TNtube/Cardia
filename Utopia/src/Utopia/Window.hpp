#pragma once

#include "utpch.hpp"
#include "Utopia/Core.hpp"
#include "Utopia/Event.hpp"


namespace Utopia
{
	struct WinProperties
	{
		int width, height;
		std::string title;

		explicit WinProperties(std::string title = "Utopia",
			int width = 1280,
			int height = 720)
			: width(width), height(height), title(std::move(title)) {}
	};
	
	class UTOPIA_API Window
	{
	public:
		virtual ~Window() = default;
		virtual void onUpdate() = 0;
		
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
		virtual std::pair<int, int> getSize() const = 0;

		virtual void setEventCallback(const std::function<void(Event&)>& callback) = 0;
		virtual void setVSync(bool state) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWin() const = 0;

		static std::unique_ptr<Window> Create(const WinProperties& properties = WinProperties());

	};
}
