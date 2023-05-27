#pragma once

#include <imgui_impl_vulkan.h>

#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Renderer.hpp"


namespace Cardia
{
	class ImGuiLayer {
	public:
		ImGuiLayer(Renderer& renderContext);
		~ImGuiLayer();

		void onEvent(Event &event) {};

		void Begin();
		void Render(VkCommandBuffer commandBuffer);
		void End();

	private:
		Renderer& m_RenderContext;
		VkDescriptorPool m_DescriptorPool {};
		float m_Time {};
	};
}
