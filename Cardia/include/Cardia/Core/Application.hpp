#pragma once

#include "Cardia/Core/Core.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Core/Window.hpp"
#include "Cardia/Layer/LayerStack.hpp"
#include "Cardia/ImGui/ImGuiLayer.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Renderer/Buffer.hpp"
#include "Cardia/Renderer/VertexArray.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Cardia/Renderer/Camera.hpp"


namespace Cardia
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
		inline void close() { m_Running = false; }

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		DeltaTime m_DeltaTime;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;

	};

	std::unique_ptr<Application> CreateApplication();
}
