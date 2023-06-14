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
		CreateRenderPass();
		CreateFramebuffers();
		CreatePool();
		
		const Application& app = Application::get();
		const auto window = static_cast<GLFWwindow*>(app.GetWindow().getNativeWin());
		const auto& device = m_Renderer.GetDevice();
		
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
		init_info.DescriptorPool = m_Pool->GetPool();
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
		ImGui_ImplVulkan_Init(&init_info, m_RenderPass);

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
		const auto& device = m_Renderer.GetDevice();
		vkDeviceWaitIdle(device.GetDevice());
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyRenderPass(m_Renderer.GetDevice().GetDevice(), m_RenderPass, nullptr);
	}

	void ImGuiLayer::Begin() const
	{
		m_Renderer.BeginRenderPass(m_Framebuffers[m_Renderer.GetCurrentImageIndex()], m_RenderPass);
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::Render(VkCommandBuffer commandBuffer)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	void ImGuiLayer::End() const
	{
		const Application& app = Application::get();
		auto[w, h] = app.GetWindow().getSize();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));

		// Rendering
		ImGui::Render();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
		m_Renderer.EndRenderPass();
	}

	void ImGuiLayer::CreateRenderPass()
	{
		VkAttachmentDescription attachment = {};
		attachment.format = m_Renderer.GetSwapChain().GetSwapChainImageFormat();
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;
		
		if (vkCreateRenderPass(m_Renderer.GetDevice().GetDevice(), &info, nullptr, &m_RenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void ImGuiLayer::CreateFramebuffers()
	{
		const auto& swapchain = m_Renderer.GetSwapChain();
		const auto extent = swapchain.GetSwapChainExtent();
		m_Framebuffers.reserve(swapchain.ImageCount());
		for (uint32_t i = 0; i < swapchain.ImageCount(); i++) {
			const std::vector attachments = {swapchain.GetImageView(i)};
			FramebufferSpecification specification {
				.width = extent.width,
				.height = extent.height,
				.attachments = attachments
			};
			m_Framebuffers.emplace_back(m_Renderer.GetDevice(), m_RenderPass, specification);
		}
	}

	void ImGuiLayer::CreatePool()
	{
		m_Pool = std::make_unique<DescriptorPool>(
			DescriptorPool::Builder(m_Renderer.GetDevice())
				.AddPoolSize( VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
				.AddPoolSize( VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100)
				.SetMaxSets(1000)
				.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
				.Build());
	}
}
