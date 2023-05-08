#pragma once

#include "cdpch.hpp"
#include "vulkan/vulkan.h"
#include "Cardia/Core/Core.hpp"
#include "Event.hpp"


namespace Cardia
{
	struct WinProperties
	{
		int width, height;
		std::string title;

		explicit WinProperties(std::string title = "Cardia",
			int width = 1920,
			int height = 1080)
			: width(width), height(height), title(std::move(title)) {}
	};
	
	class Window
	{
	public:
		virtual ~Window() = default;
		virtual void onUpdate() = 0;
		
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
		virtual std::pair<int, int> getSize() const = 0;
		virtual VkExtent2D GetExtent() const = 0;

		virtual void setEventCallback(const std::function<void(Event&)>& callback) = 0;
		virtual void setFullscreen(bool state) = 0;
		virtual bool isFullscreen() const = 0;
		virtual void setVSync(bool state) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWin() const = 0;

		static std::unique_ptr<Window> Create(const WinProperties& properties = WinProperties());

	};
}
