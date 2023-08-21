#include "Panels/InspectorPanel.hpp"

#include <filesystem>
#include <imgui.h>

#include "CardiaTor.hpp"
#include "Cardia/ECS/Components.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "EditorUI/DragData.hpp"
#include "Panels/PanelManager.hpp"
#include "Cardia/Application.hpp"
#include "Cardia/Asset/AssetsManager.hpp"
#include "Cardia/Project/Project.hpp"


namespace Cardia::Panel
{
	int InspectorPanel::m_LastWindowId = 0;
	void InspectorPanel::OnImGuiRender(CardiaTor* appContext)
	{
		char buff[64];
		sprintf(buff, "Inspector##%i", m_WindowId);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(buff, &m_IsOpen)) {
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowFocused()) {
			m_PanelManager->SetFocused<InspectorPanel>(this);
		}

		if(!m_CurrentScene)
		{
			ImGui::End();
			return;
		}

		if(!m_SelectedEntity.IsValid())
		{
			ImGui::End();
			return;
		}

		const auto& uuid = m_SelectedEntity.GetComponent<Component::ID>();

		// Label Component
		DrawInspectorComponent<Component::Label>("Label", [this](Component::Label& label)
		{
			EditorUI::InputText("Name", &label.Name, label.Color);
			EditorUI::ColorEdit4("Color", &label.Color.x);
		});

		// Transform Component

		DrawInspectorComponent<Component::Transform>("Transform", [this](Component::Transform& transform) {
			auto position = transform.GetPosition();
			if (EditorUI::DragFloat3("Position", position))
				transform.SetPosition(position);



			auto rotation = Vector3f(
				Degreef::FromRadian(transform.GetRotation().x).Value(),
				Degreef::FromRadian(transform.GetRotation().y).Value(),
				Degreef::FromRadian(transform.GetRotation().z).Value());

			if (EditorUI::DragFloat3("Rotation", rotation))
			{
				transform.SetRotation(Vector3f(
					Radianf::FromDegree(rotation.x).Value(),
					Radianf::FromDegree(rotation.y).Value(),
					Radianf::FromDegree(rotation.z).Value()));
			}

			auto scale = transform.GetScale();
			if (EditorUI::DragFloat3("Scale", scale, 1))
				transform.SetScale(scale);

			if (transform.IsDirty())
			{
				transform.RecomputeWorld(m_SelectedEntity);
			}
		});

		// SpriteRenderer Component

		DrawInspectorComponent<Component::SpriteRenderer>("Sprite Renderer", [appContext](Component::SpriteRenderer& sprite) {
			EditorUI::ColorEdit4("Color", &sprite.Color.x);

			auto& white = appContext->GetRenderer().GetWhiteTexture();
			const VkDescriptorSet texID = sprite.Texture ? sprite.Texture->GetDescriptorSet().GetDescriptor() : white.GetDescriptorSet().GetDescriptor();

			ImGui::Image(texID, {15, 15});
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
				{
					const auto* cStrPath = static_cast<const char*>(payload->Data);
					if (auto tex = AssetsManager::Load<Texture2D>(cStrPath))
					{
						sprite.Texture = std::move(tex);
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			ImGui::Text("Texture");
			EditorUI::DragInt("zIndex", &sprite.ZIndex, 0.05f);
		});


		// MeshRendererC Component

		DrawInspectorComponent<Component::MeshRendererC>("Mesh Renderer", [appContext](Component::MeshRendererC& meshRendererC) {

			auto str = AssetsManager::GetPathFromAsset(meshRendererC.Renderer->GetMesh()).string();

			EditorUI::InputText("Mesh path", &str, Vector4f(0), ImGuiInputTextFlags_ReadOnly);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
				{
					auto path = std::filesystem::path(static_cast<const char*>(payload->Data));
					auto mesh = AssetsManager::Load<Mesh>(path);
					
					meshRendererC.Renderer->SubmitMesh(appContext->GetRenderer().GetDevice(), mesh);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Text("Materials");

			if (!meshRendererC.Renderer->GetMesh()) return;
			auto& materials = meshRendererC.Renderer->GetMesh()->GetMaterials();
			for (auto& material : materials) {
				auto& white = appContext->GetRenderer().GetWhiteTexture();
				const VkDescriptorSet texID = material ? material->GetDescriptorSet().GetDescriptor() : white.GetDescriptorSet().GetDescriptor();
				ImGui::Image(texID, {15, 15});
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
					{
						const auto* cStrPath = static_cast<const char*>(payload->Data);
						
						std::filesystem::path path = cStrPath;
						if (auto texture = AssetsManager::Load<Texture2D>(path))
						{
							material = std::move(texture);
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				ImGui::Text("Texture");
			}
			const auto textWidth = ImGui::CalcTextSize("  +  ").x;

			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textWidth) * 0.5f);
			if (ImGui::Button("  +  ")) {
				materials.push_back(AssetsManager::Load<Texture2D>("resources/textures/white.jpg", AssetsManager::LoadType::Editor));
			}
		});

		// Camera Component

		DrawInspectorComponent<Component::Camera>("Camera", [](Component::Camera& camera) {
			SceneCamera& cam = camera.CameraData;

			int type = static_cast<int>(cam.GetProjectionType());
			const char *cameraTypes[] = {"Perspective", "Orthographic"};
			if (EditorUI::Combo("Camera Type", &type, cameraTypes, sizeof(cameraTypes) / sizeof(char *)))
				cam.SetProjectionType(static_cast<SceneCamera::ProjectionType>(type));

			auto& isPrimary = camera.Primary;

			EditorUI::Checkbox("Primary", &isPrimary);

			if (cam.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
				int actions = 0;

				const auto perspective = cam.GetPerspective();

				float pFov = perspective.VerticalFOV.ToDegree().Value();
				actions += EditorUI::DragFloat("Fov", &pFov, 0.05f);

				float pNear = perspective.NearClip;
				actions += EditorUI::DragFloat("Near", &pNear, 0.05f);

				float pFar = perspective.FarClip;
				actions += EditorUI::DragFloat("Far", &pFar, 0.05f);

				if (actions > 0)
					cam.SetPerspective({Radianf::FromDegree(pFov), pNear, pFar});
			}
			else if (cam.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				auto orthographic = cam.GetOrthographic();

				int actions = 0;
				actions += EditorUI::DragFloat("Size", &orthographic.Size, 0.05f);

				actions += EditorUI::DragFloat("Near", &orthographic.NearClip, 0.05f);

				actions += EditorUI::DragFloat("Far", &orthographic.FarClip, 0.05f);

				if (actions > 0)
					cam.SetOrthographic(orthographic);
			}
		});

		// Light Component

		DrawInspectorComponent<Component::Light>("Light", [](Component::Light& light) {
			int item_current = light.LightType;
			const char* items[] = { "Directional Light", "Point Light", "Spot Light" };
			EditorUI::Combo("Light Type", &item_current, items, IM_ARRAYSIZE(items));

			light.LightType = item_current;

			EditorUI::ColorEdit3("Color", &light.Color.x);

			if (item_current == 0) return;
			EditorUI::DragFloat("Range", &light.Range, 0.01f, 0.0f);

			if (item_current == 1) return;
			EditorUI::DragFloat("Angle", &light.Angle, 0.5f, 0.0f);
			EditorUI::DragFloat("Smoothness", &light.Smoothness, 0.5f);
		});

		DrawInspectorComponent<Component::Script>("Script", [&](Component::Script& scriptComponent) {
			const std::filesystem::path filepath = scriptComponent.GetPath();
			auto path = filepath.filename().string();

			EditorUI::InputText("Script Name", &path, Vector4f(1), ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
				{
					const std::filesystem::path behaviorPath = static_cast<const char*>(payload->Data);
					if (behaviorPath.extension() == ".py")
					{
						scriptComponent.SetPath(behaviorPath.string());
					}
					else
					{
						Log::Warn("Could not load file {0}", behaviorPath.string());
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (!scriptComponent.IsLoaded()) return;

			auto& file = scriptComponent.GetFile();

			auto& attributes = file.Attributes();

			for (auto& field: attributes)
			{
				if (!field.IsEditable() || field.IsNone()) continue;
				DrawField(file, field.GetName(), field.GetType());
			}
		});

		ImGui::Separator();

		const auto windowWidth = ImGui::GetWindowSize().x;
		const auto textWidth   = ImGui::CalcTextSize("Add Components...").x;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		if (ImGui::Button("Add Component...")) {
			ImGui::OpenPopup("Add Component");
		}


		// Add component button
		if (ImGui::BeginPopup("Add Component"))
		{
			if (!m_SelectedEntity.HasComponent<Component::Camera>() && ImGui::MenuItem("Camera"))
			{
				m_SelectedEntity.AddComponent<Component::Camera>();
			}

			if (!m_SelectedEntity.HasComponent<Component::SpriteRenderer>() && ImGui::MenuItem("Sprite Renderer"))
			{
				m_SelectedEntity.AddComponent<Component::SpriteRenderer>();
			}

			if (!m_SelectedEntity.HasComponent<Component::MeshRendererC>() && ImGui::MenuItem("Mesh Renderer"))
			{
				m_SelectedEntity.AddComponent<Component::MeshRendererC>();
			}

			if (!m_SelectedEntity.HasComponent<Component::Script>() && ImGui::MenuItem("Entity Behavior"))
			{
				m_SelectedEntity.AddComponent<Component::Script>();
			}

			if (!m_SelectedEntity.HasComponent<Component::Light>() && ImGui::MenuItem("Light"))
			{
				m_SelectedEntity.AddComponent<Component::Light>();
			}
			ImGui::EndPopup();
		}
		ImGui::End();
	}


	template<typename T>
	void InspectorPanel::DrawInspectorComponent(const char* name, std::function<void(T&)> func)
	{
		if (!m_SelectedEntity.HasComponent<T>())
			return;

		constexpr auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		auto& component = m_SelectedEntity.GetComponent<T>();
		if(!ImGui::TreeNodeEx(static_cast<void*>(&component), componentFlags, "%s", name))
			return;


		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Reset Component"))
			{
				component.Reset();
			}
			if (!std::is_same_v<T, Component::Transform> && ImGui::MenuItem("Remove Component"))
			{
				m_SelectedEntity.RemoveComponent<T>();
			}
			ImGui::EndPopup();
		}
		func(component);
		ImGui::TreePop();
	}

	bool InspectorPanel::DrawField(ScriptFile& file, const std::string& fieldName, ScriptFieldType type) {

		switch (type) {
			case ScriptFieldType::Int:
			{
				auto field = file.GetAttribute<int>(fieldName);
				if (EditorUI::DragInt(fieldName.c_str(), &field, 0.1f))
				{
					file.SetAttribute(fieldName, field);
					return true;
				}
				return false;
			}
			case ScriptFieldType::Bool:
			{
				auto field = file.GetAttribute<bool>(fieldName);
				if (EditorUI::Checkbox(fieldName.c_str(), &field))
				{
					file.SetAttribute(fieldName, field);
					return true;
				}
				return false;
			}
			case ScriptFieldType::Float:
			{
				auto field = file.GetAttribute<float>(fieldName);
				if (EditorUI::DragFloat(fieldName.c_str(), &field, 0.1f))
				{
					file.SetAttribute(fieldName, field);
					return true;
				}
				return false;
			}
			case ScriptFieldType::String:
			{
				auto field = file.GetAttribute<std::string>(fieldName);
				if (EditorUI::InputText(fieldName.c_str(), &field))
				{
					file.SetAttribute(fieldName, field);
					return true;
				}
				return false;
			}
			case ScriptFieldType::Vector2:
			{
				auto* field = file.GetAttribute<Vector2f*>(fieldName);
				return EditorUI::DragFloat2(fieldName, *field, 0.0f);
			}
			case ScriptFieldType::Vector3:
			{
				auto* field = file.GetAttribute<Vector3f*>(fieldName);
				return EditorUI::DragFloat3(fieldName, *field, 0.0f);
			}
			case ScriptFieldType::Vector4:
			{
				auto* field = file.GetAttribute<Vector4f*>(fieldName);
				return EditorUI::DragFloat4(fieldName, *field, 0.0f);
			}
			case ScriptFieldType::List:
			case ScriptFieldType::Dict:
			case ScriptFieldType::Tuple:
			case ScriptFieldType::PyBehavior:
			case ScriptFieldType::UnEditable:
				return false;
		}

		return false;

	}

	void InspectorPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void InspectorPanel::OnSceneLoad(Scene *scene)
	{
		IPanel::OnSceneLoad(scene);
		m_SelectedEntity = Entity();
	}
}
