#include "cdpch.hpp"
#include "Cardia/ImGui/ImGuiLayer.hpp"
#include "Cardia/Application.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>


namespace Cardia
{
	ImGuiLayer::ImGuiLayer(Renderer& renderer) : m_Renderer(renderer)
	{
		const Application& app = Application::get();
		const auto window = static_cast<GLFWwindow*>(app.GetWindow().getNativeWin());
		const auto& device = m_Renderer.m_Device;
		
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		
		ImGui_ImplGlfw_InitForVulkan(window, true);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = device.m_Instance;
		init_info.PhysicalDevice = device.m_PhysicalDevice;
		init_info.Device = device.m_Device;
		init_info.QueueFamily = device.FindQueueFamilies(device.m_PhysicalDevice).GraphicsFamily;
		init_info.Queue = device.m_GraphicsQueue;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = m_Renderer.GetDescriptorSetPool().GetPool();
		init_info.Subpass = 0;
		init_info.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = 2;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.CheckVkResultFn = [](VkResult err)
		{
			if (err == VK_SUCCESS)
				return;
			Log::coreError("Vulkan Error : VkResult = {0}", err);
		};
		ImGui_ImplVulkan_Init(&init_info, m_Renderer.m_SwapChain->GetRenderPass());

		constexpr float fontSize = 16.0f;
		io.Fonts->AddFontFromFileTTF("resources/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		auto commandBuffer = device.BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.EndSingleTimeCommands(commandBuffer);
		vkDeviceWaitIdle(device.m_Device);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	ImGuiLayer::~ImGuiLayer()
	{
		const auto& device = m_Renderer.m_Device;
		vkDeviceWaitIdle(device.GetDevice());
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::Render(VkCommandBuffer commandBuffer)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	void ImGuiLayer::End()
	{
		const Application& app = Application::get();
		auto[w, h] = app.GetWindow().getSize();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));

		// Rendering
		ImGui::Render();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

}
