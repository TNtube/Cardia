#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLShader.hpp"
#include "Cardia/Renderer/Renderer.hpp"

#include <map>
#include <set>
#include <GLFW/glfw3.h>

#include "Cardia/Core/Window.hpp"


const std::vector validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> GetRequiredInstanceExtensions();
bool CheckValidationLayerSupport();
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
bool IsDeviceSuitable(VkPhysicalDevice device);
bool CheckDeviceExtensionSupport(VkPhysicalDevice device);


namespace Cardia
{
	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::beginScene(Camera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const VertexArray* vertexArray, Shader* shader, const glm::mat4& transform)
	{
		shader->bind();
		shader->setMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->setMat4("u_Model", transform);
		shader->setMat3("u_TransposedInvertedModel", glm::transpose(glm::inverse(glm::mat3(transform))));

		vertexArray->bind();
		RenderAPI::get().drawIndexed(vertexArray);
	}
}

