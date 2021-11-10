#include "Panels/SceneHierarchy.hpp"
#include <entt/entt.hpp>


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
			auto node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, node_flags, "%s", name.name.c_str())) {
				if(ImGui::IsItemClicked(0)) {
					m_EntityClicked.reset();
					m_EntityClicked = std::make_unique<Entity>(entity, m_Scene);
				}
			}
		}
		ImGui::End();
	}

	void SceneHierarchy::drawComponents()
	{
		ImGui::Begin("Inspector");
		if(m_EntityClicked)
		{
			// Name Component
			auto& name = m_EntityClicked->getComponent<Component::Name>();

			char buffer[128] {0};
			size_t bufferSize = sizeof(buffer)/sizeof(char);
			name.name.copy(buffer, bufferSize);

			if(ImGui::InputText("Name", buffer, bufferSize))
			{
				name.name = std::string(buffer, bufferSize);
			}

			// Transform Component

			ImGui::Text("Transform");
			auto& transform = m_EntityClicked->getComponent<Component::Transform>();
			ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.05f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.05f);
			ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.05f);

			// SpriteRenderer Component

			if (m_EntityClicked->hasComponent<Component::SpriteRenderer>()) {
				ImGui::Text("Sprite Renderer");
				auto& sprite = m_EntityClicked->getComponent<Component::SpriteRenderer>();
				ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
			}

			// Camera Component

			if (m_EntityClicked->hasComponent<Component::Camera>()) {
				ImGui::Text("Camera");
				auto& camera = m_EntityClicked->getComponent<Component::Camera>();
				int type = (int)camera.camera.getProjectionType();
				const char* cameraTypes[] = { "Perspective", "Orthographic" };
				if(ImGui::Combo("Camera Type", &type, cameraTypes, sizeof(cameraTypes)/sizeof(char*)))
					camera.camera.setProjectionType((SceneCamera::ProjectionType)type);
			}
		}
		ImGui::End();
	}
}