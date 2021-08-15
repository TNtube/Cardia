#include "utpch.hpp"
#include "ImGuiLayer.hpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "Utopia/Application.hpp"

// Temporary
#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Utopia
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		
	}

	void ImGuiLayer::onPush()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// TEMPORARY
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;


		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::onPop()
	{
		
	}

	void ImGuiLayer::onUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		auto[w, h] = Application::get().getWindow().getSize();
		io.DisplaySize = ImVec2(w, h);

		const auto time = static_cast<float>(glfwGetTime());
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Log::coreInfo("Updating ImGui");
	}

	void ImGuiLayer::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MouseButtonDownEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onMouseButtonDownEvent));
		dispatcher.dispatch<MouseButtonUpEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onMouseButtonUpEvent));
		dispatcher.dispatch<MouseMotionEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onMouseMotionEvent));
		dispatcher.dispatch<MouseScrolledEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onMouseScrolledEvent));
		dispatcher.dispatch<KeyDownEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onKeyDownEvent));
		dispatcher.dispatch<KeyUpEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onKeyUpEvent));
		dispatcher.dispatch<KeyTypedEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));
		dispatcher.dispatch<WinResizeEvent>(UT_BIND_EVENT_FN(ImGuiLayer::onWinResizeEvent));
	}

	bool ImGuiLayer::onMouseButtonDownEvent(const MouseButtonDownEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getButton()] = true;

		return false;
	}

	bool ImGuiLayer::onMouseButtonUpEvent(const MouseButtonUpEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getButton()] = false;

		return false;
	}

	bool ImGuiLayer::onMouseMotionEvent(const MouseMotionEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto [x, y] = e.getPos();
		io.MousePos = ImVec2(x, y);

		return false;
	}

	bool ImGuiLayer::onMouseScrolledEvent(const MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel = e.getOffSetY();
		io.MouseWheelH = e.getOffSetX();

		return false;
	}

	bool ImGuiLayer::onKeyDownEvent(const KeyDownEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		return false;
	}

	bool ImGuiLayer::onKeyUpEvent(const KeyUpEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = false;
		return false;
	}

	bool ImGuiLayer::onKeyTypedEvent(const KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		const int c { e.getKeyCode() };
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter(static_cast<unsigned short>(c));

		return false;
	}

	bool ImGuiLayer::onWinResizeEvent(const WinResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		auto [w, h] = e.getSize();
		io.DisplaySize = ImVec2(w, h);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		glViewport(0, 0, w, h); // Temporary

		return false;
	}
}
