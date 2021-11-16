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
					m_EntityClicked = Entity(entity, m_Scene);
				}
				ImGui::TreePop();
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
			auto& name = m_EntityClicked.getComponent<Component::Name>();

			char buffer[128] {0};
			size_t bufferSize = sizeof(buffer)/sizeof(char);
			name.name.copy(buffer, bufferSize);

			if(ImGui::InputText("Name", buffer, bufferSize))
			{
				name.name = std::string(buffer, bufferSize);
			}

			auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

			// Transform Component

			drawInspectorComponent<Component::Transform>("Transform", [](Component::Transform& transform) {
				ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.05f);
				auto rotation = glm::degrees(transform.rotation);
				if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.2f))
					transform.rotation = glm::radians(rotation);

				ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.05f);
			});

			// SpriteRenderer Component

			drawInspectorComponent<Component::SpriteRenderer>("Sprite Renderer", [](Component::SpriteRenderer& sprite) {
				ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
			});

			// Camera Component

			drawInspectorComponent<Component::Camera>("Camera", [](Component::Camera& camera) {
				SceneCamera& cam = camera.camera;

				int type = static_cast<int>(cam.getProjectionType());
				const char *cameraTypes[] = {"Perspective", "Orthographic"};
				if (ImGui::Combo("Camera Type", &type, cameraTypes,
						 sizeof(cameraTypes) / sizeof(char *)))
					cam.setProjectionType(static_cast<SceneCamera::ProjectionType>(type));
				if (cam.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
					float pFov = glm::degrees(cam.getPerspectiveFov());
					if (ImGui::DragFloat("Fov", &pFov, 0.05f))
						cam.setPerspectiveFov(glm::radians(pFov));
					float pFar = cam.getPerspectiveFar();
					if (ImGui::DragFloat("Far", &pFar, 0.05f))
						cam.setPerspectiveFar(pFar);
					float pNear = cam.getPerspectiveNear();
					if (ImGui::DragFloat("Near", &pNear, 0.05f))
						cam.setPerspectiveNear(pNear);
				}
			});
		}
		ImGui::End();
	}

	template<typename T>
	void SceneHierarchy::drawInspectorComponent(const char* name, std::function<void(T&)> func)
	{
		auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (m_EntityClicked.hasComponent<T>()) {
			auto& component = m_EntityClicked.getComponent<T>();
			if(ImGui::TreeNodeEx((void*)(&component), componentFlags, "%s", name))
			{
				func(component);
				ImGui::TreePop();
			}
		}
	}
}