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

		void Begin() const;
		void Render(VkCommandBuffer commandBuffer);
		void End() const;

	private:
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreatePool();
		Renderer& m_Renderer;
		VkRenderPass m_RenderPass {};
		std::vector<Framebuffer> m_Framebuffers;
		std::unique_ptr<DescriptorPool> m_Pool;
		float m_Time {};
	};
}
