#include <utility>
#include <Cardia/Core/Time.hpp>
#include "cdpch.hpp"
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include "Cardia/Serialization/Serializer.hpp"
#include "Cardia/Application.hpp"


namespace Cardia
{


	Scene::Scene(Renderer& renderer, std::string name)
		: m_Renderer(renderer), m_Name(std::move(name)) {}

	Scene::Scene(Renderer& renderer, std::filesystem::path path) : Scene(renderer, path.filename().string())
	{
		m_Path = std::move(path);
	}

	Scene::Scene(Scene&& other) noexcept : m_Renderer(other.m_Renderer), m_Name(std::move(other.m_Name))
	{
		CopyRegistry(other);
	}

	Scene& Scene::operator=(Scene&& other) noexcept
	{
		CopyRegistry(other);
		return *this;
	}

	Scene::Scene(const Scene& other)
		: Scene(other.m_Renderer, other.m_Name)
	{
		CopyRegistry(other);
	}

	Scene& Scene::operator=(const Scene& other)
	{
		CopyRegistry(other);
		return *this;
	}

	Scene::~Scene()
	{
		// TODO : Move this to assets manager
		m_Renderer.GetDevice().WaitIdle();
	}

	Entity Scene::CreateEntity(const std::string& name, entt::entity parent)
	{
		Entity entity = {m_Registry.create(), this};
		PopulateDefaultEntity(entity, name, UUID(), parent);
		return entity;
	}

	Entity Scene::CreateEntityFromId(UUID uuid, entt::entity parent)
	{
		Entity entity = {m_Registry.create(), this};
		PopulateDefaultEntity(entity, "Entity", uuid, parent);
		return entity;
	}

	void Scene::DestroyEntity(const Entity entity)
	{
		const auto& relationship = entity.GetComponent<Component::Relationship>();
		auto current = Entity(relationship.FirstChild, this);
		while (current.IsValid())
		{
			const auto next = current.GetComponent<Component::Relationship>().NextSibling;
			DestroyEntity(current);
			current = Entity(next, this);
		}
		m_Registry.destroy(entity.Handle());
	}

	void Scene::OnRuntimeRender(VkCommandBuffer commandBuffer)
	{
		SceneCamera* mainCamera = nullptr;
		Component::Transform* mainCameraTransform = nullptr;

		{
			const auto viewCamera = m_Registry.view<Component::Transform, Component::Camera>();
			for (const auto entity: viewCamera)
			{
				auto[transform, cam] = viewCamera.get<Component::Transform, Component::Camera>(
					entity);
				if (cam.Primary)
				{
					mainCamera = &cam.CameraData;
					mainCameraTransform = &transform;
				}
			}
		}

		if (!mainCamera || !mainCameraTransform)
		{
			Log::Error("Scene hierarchy should have a primary camera. Either create one or set the existing one to primary");
			return;
		}
		OnRender(commandBuffer, *mainCamera, *mainCameraTransform);
	}

	void Scene::OnRender(VkCommandBuffer commandBuffer, Camera& camera, const Component::Transform& cameraTransform)
	{
		auto& frame = m_Renderer.GetCurrentFrame();

		// Render Skybox
		auto& skybox = m_Renderer.GetSkybox();
		auto& skyboxPipeline = skybox.GetPipeline();
		skyboxPipeline.Bind(commandBuffer);
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			skyboxPipeline.GetLayout(),
			0, 1,
			&frame.SkyboxUboDescriptorSet->GetDescriptor(),
			0, nullptr);
		SkyboxUboData skyboxUboData {};
		skyboxUboData.Projection = camera.GetProjectionMatrix();
		skyboxUboData.Model = cameraTransform.GetWorldTransform().Inverse();
		skyboxUboData.Model[3] = Vector4f(0, 0, 0, 1);
		frame.SkyboxUboBuffer->UploadData(sizeof(SkyboxUboData), &skyboxUboData);

		skybox.Draw(commandBuffer);

		// Render Meshes
		m_Renderer.GetMainPipeline().Bind(commandBuffer);
		const auto meshView = m_Registry.view<Component::Transform, Component::ModelRenderer>();
		if (meshView.size_hint() > 0)
		{
			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				m_Renderer.GetMainPipeline().GetLayout(),
				0, 1,
				&frame.MainUboDescriptorSet->GetDescriptor(),
				0, nullptr);
			UboData data {};
			data.ViewProjection = camera.GetProjectionMatrix() * cameraTransform.GetWorldTransform().Inverse();
			data.CameraPositionAndTime = Vector4f(cameraTransform.GetPosition(), Time::GetTime());
			frame.MainUboBuffer->UploadData(sizeof(UboData), &data);
		}
		for (const auto entity : meshView)
		{
			auto [transform, meshRenderer] = meshView.get<Component::Transform, Component::ModelRenderer>(entity);
			// m_UBO->bind(0);
			PushConstantData constants {};
			constants.Model = transform.GetWorldTransform();
			constants.TransposedInvertedModel = constants.Model.Inverse().Transpose();
			vkCmdPushConstants(
				commandBuffer,
				m_Renderer.GetMainPipeline().GetLayout(),
				VK_SHADER_STAGE_VERTEX_BIT,
				0, sizeof(PushConstantData),
				&constants);

			if (meshRenderer.Renderer)
				meshRenderer.Renderer->Draw(m_Renderer.GetMainPipeline(), commandBuffer);
		}
	}

	void Scene::OnViewportResize(float width, float height)
	{
		const auto view = m_Registry.view<Component::Camera>();
		for (const auto entity : view)
		{
			auto& cameraComponent = view.get<Component::Camera>(entity);
			cameraComponent.CameraData.SetViewportSize(width, height);
		}
	}

	void Scene::Clear()
	{
		m_Registry.clear();
	}

	void Scene::OnRuntimeStart()
	{
		ScriptEngine::Instance().OnRuntimeStart(this);
	}

	Entity Scene::GetEntityByUUID(const UUID& uuid)
	{
		const auto view = m_Registry.view<UUID>();
		for (const auto entity : view)
		{
			auto& idComponent = view.get<UUID>(entity);
			if (idComponent == uuid) {
				return {entity, this};
			}
		}
		return {entt::null, this};
	}

	bool Scene::IsEntityValid(entt::entity entity) const
	{
		return m_Registry.valid(entity);
	}

	void Scene::OnRuntimeStop()
	{
		ScriptEngine::Instance().OnRuntimeEnd();
	}

	std::unique_ptr<Scene> Scene::Copy(Scene& src)
	{
		return std::make_unique<Scene>(src);
	}

	// Recursively copy the values of b into a. Both a and b must be objects.
	static void MergeJson(Json::Value& dst, const Json::Value& src) {
		if (!dst.isObject() || !src.isObject()) return;

		for (const auto& key : src.getMemberNames()) {
			if (dst[key].isObject()) {
				MergeJson(dst[key], src[key]);
			} else {
				dst[key] = src[key];
			}
		}
	}

	Json::Value Scene::Serialize() const
	{
		Json::Value root;

		auto& entitiesNode = root["Entities"];
		for(const auto entity: m_Registry.view<entt::entity>())
		{
			Entity currEntity(entity, const_cast<Scene*>(this));
			Json::Value currentEntityNode = currEntity.SerializeComponents();

			// Serialize Relationships here as they need scene context to be deserialized
			const auto& relationship = m_Registry.get<Component::Relationship>(entity);
			Json::Value toMerge(Json::objectValue);
			auto& relationshipNode = toMerge["Relationship"];
			relationshipNode["ChildCount"] = relationship.ChildCount;
			if (m_Registry.valid(relationship.Parent))
				relationshipNode["Parent"] = m_Registry.get<UUID>(relationship.Parent).ToString();
			if (m_Registry.valid(relationship.FirstChild))
				relationshipNode["FirstChild"] = m_Registry.get<UUID>(relationship.FirstChild).ToString();
			if (m_Registry.valid(relationship.PreviousSibling))
				relationshipNode["PreviousSibling"] = m_Registry.get<UUID>(relationship.PreviousSibling).ToString();
			if (m_Registry.valid(relationship.NextSibling))
				relationshipNode["NextSibling"] = m_Registry.get<UUID>(relationship.NextSibling).ToString();

			MergeJson(currentEntityNode, toMerge);
			entitiesNode.append(currentEntityNode);
		}

		return root;

	}

	Scene Scene::Deserialize(const Json::Value& root)
	{
		Scene scene(Application::Get().GetRenderer(), std::string("Deserialized Scene"));
		if (!root.isMember("Entities"))
			return scene;

		// Deserialize all serializable components
		for (auto& entityNode : root["Entities"])
		{
			const auto entity = scene.m_Registry.create();
			Entity::DeserializeAndAssignComponents(entityNode, scene.m_Registry, entity);
		}

		// Second pass to deserialize things that need scene context
		for (auto& entityNode : root["Entities"])
		{
			auto id = Serialization::FromJson<UUID>(entityNode);

			auto entity = scene.GetEntityByUUID(id);

			auto& relationship = entity.AddComponent<Component::Relationship>();

			if (!entityNode.isMember("Relationship"))
				continue;
			
			auto& relationshipNode = entityNode["Relationship"];
			relationship.ChildCount = relationshipNode["ChildCount"].asInt();
			if (relationshipNode.isMember("Parent"))
				relationship.Parent = scene.GetEntityByUUID(UUID::FromString(relationshipNode["Parent"].asString())).Handle();
			if (relationshipNode.isMember("FirstChild"))
				relationship.FirstChild = scene.GetEntityByUUID(UUID::FromString(relationshipNode["FirstChild"].asString())).Handle();
			if (relationshipNode.isMember("PreviousSibling"))
				relationship.PreviousSibling = scene.GetEntityByUUID(UUID::FromString(relationshipNode["PreviousSibling"].asString())).Handle();
			if (relationshipNode.isMember("NextSibling"))
				relationship.NextSibling = scene.GetEntityByUUID(UUID::FromString(relationshipNode["NextSibling"].asString())).Handle();
		}

		// Finally, compute world transforms
		const auto view = scene.m_Registry.view<Component::Transform>();
		for (const auto entity : view)
		{
			auto& transform = scene.m_Registry.get<Component::Transform>(entity);
			transform.RecomputeWorld({entity, &scene});
		}

		return scene;
	}

	void Scene::PopulateDefaultEntity(Entity& entity, std::string name, UUID uuid, const entt::entity parent)
	{
		if (!m_Registry.valid(entity.Handle()))
			return;
		auto& transform = entity.AddComponent<Component::Transform>();
		entity.AddComponent<UUID>(uuid);
		entity.AddComponent<Component::Label>(name);
		auto& relationship = entity.AddComponent<Component::Relationship>();
		relationship.Parent = parent;

		if (parent != entt::null)
		{
			auto& parentRelationship = m_Registry.get<Component::Relationship>(parent);
			auto current = parentRelationship.FirstChild;

			if (!m_Registry.valid(current))
				parentRelationship.FirstChild = entity.Handle();
			else {
				while (m_Registry.valid(m_Registry.get<Component::Relationship>(current).NextSibling))
				{
					current = m_Registry.get<Component::Relationship>(current).NextSibling;
				}
				m_Registry.get<Component::Relationship>(current).NextSibling = entity.Handle();
				relationship.PreviousSibling = current;
			}
			parentRelationship.ChildCount++;
			transform.RecomputeWorld(entity);
		}
	}

	template <typename... Cpn>
	static void ValidateStorage(ComponentGroup<Cpn...>, entt::registry& registry)
	{
		(static_cast<void>(registry.storage<Cpn>()), ...);
	}

	void Scene::CopyRegistry(const Scene& other)
	{
		ValidateStorage(AllComponents{}, m_Registry);
		for(const auto entity: other.m_Registry.view<entt::entity>()) {
			
			const auto copy = m_Registry.create();
			for (auto [id, storage] : other.m_Registry.storage())
			{
				auto* otherStorage = m_Registry.storage(id);
				if (otherStorage && storage.contains(entity))
				{
					otherStorage->push(copy, storage.value(entity));
				}
			}
		}
	}
}
