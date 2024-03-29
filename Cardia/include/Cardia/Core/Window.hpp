#pragma once

#include "cdpch.hpp"
#include "vulkan/vulkan.h"
#include "Cardia/Core/Core.hpp"
#include "Event.hpp"
#include "Cardia/Math/Vector2.hpp"


namespace Cardia
{
	struct WinProperties
	{
		int Width, Height;
		std::string Title;
		std::filesystem::path IconPath;

		explicit WinProperties(std::string title = "Cardia",
			std::filesystem::path iconPath = "resources/logo/logo.png",
			int width = 1920,
			int height = 1080)
			: Width(width), Height(height), Title(std::move(title)), IconPath(std::move(iconPath)) {}
	};
	
	class Window
	{
	public:
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual Vector2i GetSize() const = 0;
		virtual VkExtent2D GetExtent() const = 0;
		virtual bool ShouldInvalidateSwapchain() const = 0;
		virtual void SwapchainInvalidated() = 0;

		virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;
		virtual void SetFullscreen(bool state) = 0;
		virtual bool IsFullscreen() const = 0;
		virtual bool IsFocused() const = 0;
		virtual void UpdateFullscreenMode() = 0;
		virtual void SetVSync(bool state) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static std::unique_ptr<Window> Create(const WinProperties& properties = WinProperties());

	};
}
