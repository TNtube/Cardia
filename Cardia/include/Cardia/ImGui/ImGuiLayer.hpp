#pragma once

#include <imgui_impl_vulkan.h>

#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/RenderContext.hpp"


namespace Cardia
{
	class ImGuiLayer {
	public:
		ImGuiLayer(RenderContext& renderContext);
		~ImGuiLayer();

		void onEvent(Event &event) {};

		void Begin();
		void End();

	private:
		RenderContext& m_RenderContext;
		VkDescriptorPool m_DescriptorPool {};
		float m_Time {};
	};
}
