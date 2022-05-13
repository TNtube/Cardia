#include "Panels/SceneHierarchy.hpp"

#include <entt/entt.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>


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
			if (ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), node_flags, "%s", name.name.c_str())) {
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
					m_EntityClicked = Entity(entity, m_Scene);
				}
				if (ImGui::BeginPopupContextItem())
				{
					m_EntityClicked = Entity(entity, m_Scene);
					if (ImGui::MenuItem("Delete Entity"))
					{
						m_Scene->destroyEntity(entity);
						resetEntityClicked();
					}
					ImGui::EndPopup();
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			resetEntityClicked();
		}
		if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
		{
			if (ImGui::MenuItem("Create Entity"))
				m_Scene->createEntity();

			ImGui::EndPopup();
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
				else if (cam.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
					float oSize = cam.getOrthographicSize();
					if (ImGui::DragFloat("Size", &oSize, 0.05f))
						cam.setOrthographicSize(oSize);
					float oFar = cam.getOrthographicFar();
					if (ImGui::DragFloat("Far", &oFar, 0.05f))
						cam.setOrthographicFar(oFar);
					float oNear = cam.getOrthographicNear();
					if (ImGui::DragFloat("Near", &oNear, 0.05f))
						cam.setOrthographicNear(oNear);
				}
			});

			ImGui::Separator();

			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth   = ImGui::CalcTextSize("Add Components...").x;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			if (ImGui::Button("Add Component...")) {
				ImGui::OpenPopup("Add Component");
			}
			drawPopupAddComponent();
		}
		ImGui::End();
	}

	template<typename T>
	void SceneHierarchy::drawInspectorComponent(const char* name, std::function<void(T&)> func)
	{
		auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (m_EntityClicked.hasComponent<T>()) {
			auto& component = m_EntityClicked.getComponent<T>();
			if(ImGui::TreeNodeEx(static_cast<void*>(&component), componentFlags, "%s", name))
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Reset Component"))
					{
						component.reset();
						ImGui::EndPopup();
					}

					if (!std::is_same<T, Component::Transform>::value && ImGui::MenuItem("Remove Component"))
					{
						m_EntityClicked.removeComponent<T>();
						ImGui::EndPopup();
					}
					ImGui::EndPopup();
				}
				func(component);
				ImGui::TreePop();
			}
		}
	}

	void SceneHierarchy::drawPopupAddComponent()
	{
		if (ImGui::BeginPopup("Add Component"))
		{
			if (!m_EntityClicked.hasComponent<Component::Camera>() && ImGui::MenuItem("Camera"))
			{
				m_EntityClicked.addComponent<Component::Camera>();
				ImGui::EndPopup();
			}

			if (!m_EntityClicked.hasComponent<Component::SpriteRenderer>() && ImGui::MenuItem("Sprite Renderer"))
			{
				m_EntityClicked.addComponent<Component::SpriteRenderer>();
				ImGui::EndPopup();
			}
		}
	}
}