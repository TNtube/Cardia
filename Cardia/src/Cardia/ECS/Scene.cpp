#include <Cardia/Serialization/SceneSerializer.hpp>
#include <Cardia/Project/AssetsManager.hpp>
#include <utility>
#include "cdpch.hpp"
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"


namespace Cardia
{
	struct UboData
	{
		glm::mat4 viewProjection;
		glm::mat4 model;
		glm::mat4 transposedInvertedModel;
	};


	Scene::Scene(Renderer& renderer, std::string name)
		: m_Renderer(renderer), m_Name(std::move(name))
	{
		std::string shaderName = "basic";
		const auto shaderPath = "resources/shaders/" + shaderName;

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_Renderer.GetDevice().GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan : Failed to create pipeline layout");
		}
		
		PipelineConfigInfo pipelineConfig = Pipeline::DefaultPipelineConfigInfo(m_Renderer.GetSwapChain().Width(), m_Renderer.GetSwapChain().Height());
		pipelineConfig.renderPass = m_Renderer.GetSwapChain().GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(
			m_Renderer.GetDevice(),
			"resources/shaders/simple.vert.spv",
			"resources/shaders/simple.frag.spv",
			pipelineConfig
		);
		
		m_UBO = std::make_unique<Buffer>(
			m_Renderer.GetDevice(),
			sizeof(UboData),
			SwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}

	Scene::Scene(Renderer& renderer, std::filesystem::path path) : Scene(renderer, path.filename().string())
	{
		m_Path = std::move(path);
	}

	Scene::~Scene()
	{
		vkDestroyPipelineLayout(m_Renderer.GetDevice().GetDevice(), m_PipelineLayout, nullptr);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.addComponent<Component::Transform>();
		entity.addComponent<Component::ID>();
		entity.addComponent<Component::Name>(name.empty() ? "Default Entity" : name);
		return entity;
	}

	Entity Scene::CreateEntityFromId(UUID uuid) {
		Entity entity = {m_Registry.create(), this};
		entity.addComponent<Component::Transform>();
		entity.addComponent<Component::ID>(uuid);
		entity.addComponent<Component::Name>("Default Entity");
		return entity;
	}

	void Scene::DestroyEntity(entt::entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeRender(VkCommandBuffer commandBuffer)
	{
		SceneCamera* mainCamera = nullptr;
		glm::mat4 mainCameraTransform;

		{
			const auto viewCamera = m_Registry.view<Component::Transform, Component::Camera>();
			for (const auto entity: viewCamera)
			{
				auto[transform, cam] = viewCamera.get<Component::Transform, Component::Camera>(
					entity);
				if (cam.primary)
				{
					mainCamera = &cam.camera;
					mainCameraTransform = transform.getTransform();
				}
			}
		}

		if (!mainCamera)
		{
			Log::error("Scene hierarchy should have a primary camera. Either create one or set the existing one to primary");
			return;
		}
		OnRender(commandBuffer, *mainCamera, mainCameraTransform);
	}

	void Scene::OnRender(VkCommandBuffer commandBuffer, Camera& camera, const glm::mat4& cameraTransform)
	{
		// Renderer2D::beginScene(editorCamera, editorCameraTransform);
		//
		// const auto view = m_Registry.view<Component::Transform, Component::SpriteRenderer>();
		// for (const auto entity : view)
		// {
		// 	auto [transform, spriteRenderer] = view.get<Component::Transform, Component::SpriteRenderer>(entity);
		// 	Renderer2D::drawRect(transform.getTransform(), spriteRenderer.texture.get(), spriteRenderer.color, spriteRenderer.tillingFactor, spriteRenderer.zIndex, static_cast<float>(entity));
		// }
		//
		// const auto lightView = m_Registry.view<Component::Transform, Component::Light>();
		// for (const auto entity : lightView)
		// {
		// 	auto [transform, light] = lightView.get<Component::Transform, Component::Light>(entity);
		// 	Renderer2D::addLight(transform, light);
		// }
		//
		// Renderer2D::endScene();

		// m_BasicShader->bind();
		// m_BasicShader->setInt("u_Texture", 0);
		m_Pipeline->Bind(commandBuffer);

		const auto meshView = m_Registry.view<Component::Transform, Component::MeshRendererC>();
		for (const auto entity : meshView)
		{
			auto [transform, meshRenderer] = meshView.get<Component::Transform, Component::MeshRendererC>(entity);
			// m_UBO->bind(0);
			UboData data {};
			data.viewProjection = camera.getProjectionMatrix() * glm::inverse(cameraTransform);
			data.model = transform.getTransform();
			data.transposedInvertedModel = glm::transpose(glm::inverse(transform.getTransform()));
			m_UBO->UploadDataAtIndex(&data, m_Renderer.GetFrameIndex());
			meshRenderer.meshRenderer->Draw(commandBuffer);
		}
	}

	void Scene::OnViewportResize(float width, float height)
	{
		auto view = m_Registry.view<Component::Camera>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<Component::Camera>(entity);
			cameraComponent.camera.SetViewportSize(width, height);
		}
	}

	void Scene::clear()
	{
		m_Registry.clear();
	}

	void Scene::OnRuntimeStart()
	{
		ScriptEngine::Instance().OnRuntimeStart(this);
	}

	Entity Scene::GetEntityByUUID(const UUID& uuid)
	{
		auto view = m_Registry.view<Component::ID>();
		Entity result;
		for (auto entity : view)
		{
			auto& idComponent = view.get<Component::ID>(entity);
			if (idComponent.uuid == uuid) {
				result = Entity(entity, this);
			}
		}
		return result;
	}

	void Scene::OnRuntimeStop()
	{
		ScriptEngine::Instance().OnRuntimeEnd();
	}

	std::unique_ptr<Scene> Scene::Copy(Scene& src)
	{
		std::unique_ptr<Scene> dst = std::make_unique<Scene>(src.m_Renderer, src.m_Name);

		Serialization::SceneSerializer srcSerializer(src);
		Serialization::SceneSerializer dstSerializer(*dst);

		Json::Value root;
		srcSerializer.Serialize(root);
		dstSerializer.Deserialize(dst->m_Renderer, root);

		return std::move(dst);
	}
}
