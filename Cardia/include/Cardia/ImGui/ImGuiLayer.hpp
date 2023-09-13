#pragma once

#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Renderer.hpp"


namespace Cardia
{
	class ImGuiLayer {
	public:
		explicit ImGuiLayer(Renderer& renderer);
		~ImGuiLayer();

		void OnEvent(Event &event) {}

		void Begin() const;
		void Render(VkCommandBuffer commandBuffer);
		void End() const;

	private:
		void CreatePool();
		Renderer& m_Renderer;
		std::unique_ptr<DescriptorPool> m_Pool;
	};
}
