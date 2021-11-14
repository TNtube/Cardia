#include "Panels/SceneHierarchy.hpp"
#include <entt/entt.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Cardia :: Panel
{

	SceneHierarchy::SceneHierarchy(Scene* scene)
		: m_Scene(scene)
	{

	}

	void SceneHierarchy::onImGuiRender(DeltaTime deltaTime)
	{
		drawHierarchy();
		drawComponents();
	}

	void SceneHierarchy::drawHierarchy()
	{
		ImGui::Begin(m_Scene->getName());
		auto view = m_Scene->m_Registry.view<Component::Name>();

		for (auto entity : view)
		{
			auto name = view.get<Component::Name>(entity);
			auto node_flags = ((m_EntityClicked == entity) ? ImGuiTreeNodeFlags_Selected : 0);
			node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			//node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, node_flags, "%s", name.name.c_str())) {
				if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
					m_EntityClicked = entity;
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void SceneHierarchy::drawComponents()
	{
		ImGui::Begin("Inspector");
		if(m_EntityClicked != entt::null)
		{
			Entity entityClicked(m_EntityClicked, m_Scene);
			// Name Component
			auto& name = entityClicked.getComponent<Component::Name>();

			char buffer[128] {0};
			size_t bufferSize = sizeof(buffer)/sizeof(char);
			name.name.copy(buffer, bufferSize);

			if(ImGui::InputText("Name", buffer, bufferSize))
			{
				name.name = std::string(buffer, bufferSize);
			}

			auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

			// Transform Component

			auto& transform = entityClicked.getComponent<Component::Transform>();
			if(ImGui::TreeNodeEx((void*)(&transform), componentFlags, "Transform"))
			{
				ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.05f);
				auto rotation = glm::degrees(transform.rotation);
				if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.2f))
					transform.rotation = glm::radians(rotation);

				ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.05f);
				ImGui::TreePop();
			}
			// SpriteRenderer Component

			if (entityClicked.hasComponent<Component::SpriteRenderer>()) {
				auto& sprite = entityClicked.getComponent<Component::SpriteRenderer>();
				if(ImGui::TreeNodeEx((void*)(&sprite), componentFlags, "Sprite Renderer"))
				{
					ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
					ImGui::TreePop();
				}
			}

			// Camera Component

			if (entityClicked.hasComponent<Component::Camera>()) {
				auto& camera = entityClicked.getComponent<Component::Camera>();
				if(ImGui::TreeNodeEx((void*)(&camera), componentFlags, "Camera"))
				{
					int type = (int) camera.camera.getProjectionType();
					const char *cameraTypes[] = {"Perspective", "Orthographic"};
					if (ImGui::Combo("Camera Type", &type, cameraTypes,
							 sizeof(cameraTypes) / sizeof(char *)))
						camera.camera.setProjectionType((SceneCamera::ProjectionType) type);
				}
			}
		}
		ImGui::End();
	}
}