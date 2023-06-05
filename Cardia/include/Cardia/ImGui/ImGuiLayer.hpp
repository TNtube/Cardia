#pragma once

#include <imgui_impl_vulkan.h>

#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Renderer.hpp"


namespace Cardia
{
	class ImGuiLayer {
	public:
		ImGuiLayer(Renderer& renderer);
		~ImGuiLayer();

		void OnEvent(Event &event) {}

		void Begin();
		void Render(VkCommandBuffer commandBuffer);
		void End();

	private:
		Renderer& m_Renderer;
		std::unique_ptr<DescriptorPool> m_Pool;
		float m_Time {};
	};
}
