#pragma once

#include "Utopia/Core.hpp"
#include "Utopia/Window.hpp"
#include "Utopia/Layer/LayerStack.hpp"
#include "Utopia/ImGui/ImGuiLayer.hpp"
#include "Utopia/Renderer/Shader.hpp"
#include "Utopia/Renderer/Buffer.hpp"
#include "Utopia/Renderer/VertexArray.hpp"
#include "Utopia/Renderer/Renderer.hpp"


namespace Utopia
{
	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();
		void onEvent(Event& e);
		bool onWinClose(WinCloseEvent& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline static Application& get() { return *s_Instance; }
		inline Window& getWindow() const { return *m_Window; }
		inline Renderer& getRenderer() const { return *m_Renderer; }

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::unique_ptr<Renderer> m_Renderer;
	};

	std::unique_ptr<Application> CreateApplication();
}
