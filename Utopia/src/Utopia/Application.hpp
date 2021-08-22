#pragma once

#include "Utopia/Core.hpp"
#include "Utopia/Window.hpp"
#include "Utopia/Layer/LayerStack.hpp"
#include "Utopia/ImGui/ImGuiLayer.hpp"
#include "Utopia/Renderer/Shader.hpp"
#include "Utopia/Renderer/Buffer.hpp"


namespace Utopia
{
	class UTOPIA_API Application
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

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;

		// Temporary
		unsigned m_VertexArray{};
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};

	std::unique_ptr<Application> CreateApplication();
}
