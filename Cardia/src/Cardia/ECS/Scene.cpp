#include <Cardia/Serialization/SceneSerializer.hpp>
#include <Cardia/Asset/AssetsManager.hpp>
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


	Scene::Scene(Renderer& renderer, std::string name)
		: m_Renderer(renderer), m_Name(std::move(name))
	{
		std::string shaderName = "basic";
		const auto shaderPath = "resources/shaders/" + shaderName;
	}

	Scene::Scene(Renderer& renderer, std::filesystem::path path) : Scene(renderer, path.filename().string())
	{
		m_Path = std::move(path);
	}

	Scene::~Scene()
	{
		// TODO : Move this to assets manager
		vkDeviceWaitIdle(m_Renderer.GetDevice().GetDevice());
	};

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
		m_Renderer.GetPipeline().Bind(commandBuffer);
		auto& frame = m_Renderer.GetCurrentFrame();

		const auto meshView = m_Registry.view<Component::Transform, Component::MeshRendererC>();
		if (meshView.size_hint() > 0)
		{
			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				m_Renderer.GetPipelineLayout().GetPipelineLayout(),
				0, 1,
				&frame.UboDescriptorSet->GetDescriptor(),
				0, nullptr);
			UboData data {};
			data.ViewProjection = camera.getProjectionMatrix() * glm::inverse(cameraTransform);
			frame.UboBuffer->UploadData(sizeof(UboData), &data);
		}
		for (const auto entity : meshView)
		{
			auto [transform, meshRenderer] = meshView.get<Component::Transform, Component::MeshRendererC>(entity);
			// m_UBO->bind(0);
			PushConstantData constants {};
			constants.Model = transform.getTransform();
			constants.TransposedInvertedModel = glm::transpose(glm::inverse(transform.getTransform()));
			vkCmdPushConstants(
				commandBuffer,
				m_Renderer.GetPipelineLayout().GetPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT,
				0, sizeof(PushConstantData),
				&constants);

			m_Renderer.GetWhiteTexture().Bind(commandBuffer);
			meshRenderer.meshRenderer->Draw(commandBuffer);
		}
	}

	void Scene::OnViewportResize(float width, float height)
	{
		const auto view = m_Registry.view<Component::Camera>();
		for (const auto entity : view)
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
