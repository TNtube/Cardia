#include "cdpch.hpp"

#include "Cardia/ECS/Components.hpp"
#include <json/json.h>
#include <Cardia/Serialization/SceneSerializer.hpp>
#include <Cardia/Project/Project.hpp>
#include <Cardia/Project/AssetsManager.hpp>


namespace Json {
	using namespace Cardia;
	template <> glm::vec2 Value::as<glm::vec2>() const {
		glm::vec3 out;
		auto& node = *this;
		out.x = node["x"].asFloat();
		out.y = node["y"].asFloat();
		return out;
	}

	template <> glm::vec3 Value::as<glm::vec3>() const {
		glm::vec3 out;
		auto& node = *this;
		out.x = node["x"].asFloat();
		out.y = node["y"].asFloat();
		out.z = node["z"].asFloat();
		return out;
	}

	template <> glm::vec4 Value::as<glm::vec4>() const {
		glm::vec4 out;
		auto& node = *this;
		out.x = node["x"].asFloat();
		out.y = node["y"].asFloat();
		out.z = node["z"].asFloat();
		out.w = node["w"].asFloat();
		return out;
	}

	template <> Cardia::ScriptField Value::as<ScriptField>() const {
		auto& node = *this;
		Cardia::ScriptField out;

		out.name = node["name"].asString();
		out.type = static_cast<ScriptFieldType>(node["type"].asInt());

		switch (out.type) {
			case ScriptFieldType::Int:
				out.instance = py::cast(node["value"].asInt());
				break;
			case ScriptFieldType::Float:
				out.instance = py::cast(node["value"].asFloat());
				break;
			case ScriptFieldType::String:
				out.instance = py::cast(node["value"].asString());
				break;
			case ScriptFieldType::List:
			{
				py::list list;
				for (auto& subNode: node["value"]) {
					auto subField = subNode.as<ScriptField>();
					out.valueType = subField.type;
					list.attr("append")(subField.instance.object());
				}
				out.instance = list;
				break;
			}
			case ScriptFieldType::Dict:break;
			case ScriptFieldType::PyBehavior:
				out.instance = py::cast(node["value"].asString());
				break;
			case ScriptFieldType::Vector2:
				out.instance = py::cast(node["value"].as<glm::vec2>());
				break;
			case ScriptFieldType::Vector3:
				out.instance = py::cast(node["value"].as<glm::vec3>());
				break;
			case ScriptFieldType::Vector4:
				out.instance = py::cast(node["value"].as<glm::vec4>());
				break;
			case ScriptFieldType::Unserializable:break;
		}
		return out;
	}
}


namespace Cardia::Serialization
{

	Json::Value ToJson(const glm::vec2& value) {
		Json::Value out;
		out["x"] = value.x;
		out["y"] = value.y;
		return out;
	}

	Json::Value ToJson(const glm::vec3& value) {
		Json::Value out;
		out["x"] = value.x;
		out["y"] = value.y;
		out["z"] = value.z;
		return out;
	}

	Json::Value ToJson(const glm::vec4& value) {
		Json::Value out;
		out["x"] = value.x;
		out["y"] = value.y;
		out["z"] = value.z;
		out["w"] = value.w;
		return out;
	}

	Json::Value ToJson(const ScriptField& field) {
		Json::Value out;
		out["type"] = static_cast<int>(field.type);
		out["name"] = field.name;

		switch (field.type) {
			case ScriptFieldType::Int:
				out["value"] = py::handle(field.instance).cast<int>();
				break;
			case ScriptFieldType::Float:
				out["value"] = py::handle(field.instance).cast<float>();
				break;
			case ScriptFieldType::String:
				out["value"] = py::handle(field.instance).cast<std::string>();
				break;
			case ScriptFieldType::List:
			{
				py::list list(field.instance);
				for (int index = 0; index < py::len(list); index++) {
					ScriptField subField;
					subField.name = std::to_string(index);
					subField.instance = py::object(list[index]);
					switch (subField.valueType) {
						case ScriptFieldType::List:
						case ScriptFieldType::Dict:
							subField.type = ScriptFieldType::Unserializable;
							break;
						default:
							subField.type = field.valueType;
							break;
					}
					out["value"].append(ToJson(subField));
				}
				break;
			}
			case ScriptFieldType::Dict:break;
			case ScriptFieldType::PyBehavior:
				out["value"] = py::handle(field.instance).cast<std::string>();
				break;
			case ScriptFieldType::Vector2:
				out["value"] = ToJson(py::handle(field.instance).cast<glm::vec2>());
				break;
			case ScriptFieldType::Vector3:
				out["value"] = ToJson(py::handle(field.instance).cast<glm::vec3>());
				break;
			case ScriptFieldType::Vector4:
				out["value"] = ToJson(py::handle(field.instance).cast<glm::vec4>());
				break;
			case ScriptFieldType::Unserializable:break;
		}

		return out;

	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::ID& component)
	{
		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::ID::ClassName()] = std::string(component.uuid);
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Transform& component)
	{
		Json::Value node;
		node["position"] = ToJson(component.position);
		node["rotation"] = ToJson(component.rotation);
		node["scale"] = ToJson(component.scale);

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Transform::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Name& component)
	{

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Name::ClassName()] = component.name;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::MeshRendererC& component)
	{

		Json::Value node;

		node["path"] = AssetsManager::GetPathFromAsset(component.meshRenderer->GetMesh()).string();
		Json::Value materials;
		for (const auto& material : component.meshRenderer->GetMesh()->GetMaterials()) {
			materials.append(AssetsManager::GetPathFromAsset(material).string());
		}
		node["materials"] = materials;

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::MeshRendererC::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::SpriteRenderer& component)
	{

		Json::Value node;

		node["color"] = ToJson(component.color);
		node["texture"] = AssetsManager::GetPathFromAsset(component.texture).string();
		node["tillingFactor"] = component.tillingFactor;
		node["zIndex"] = component.zIndex;

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::SpriteRenderer::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Camera &component)
	{

		Json::Value node;

		node["type"] = static_cast<int>(component.camera.GetProjectionType());
		auto pers = component.camera.GetPerspective();
		node["perspectiveFov"] = pers.x;
		node["perspectiveNear"] = pers.y;
		node["perspectiveFar"] = pers.z;

		auto ortho = component.camera.GetOrthographic();
		node["orthoSize"] = ortho.x;
		node["orthoNear"] = ortho.y;
		node["orthoFar"] = ortho.z;

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Camera::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Light &component)
	{

		Json::Value node;

		node["type"] = component.lightType;
		node["color"] = ToJson(component.color);

		node["range"] = component.range;
		node["angle"] = component.angle;
		node["smoothness"] = component.smoothness;

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Light::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Script &component)
	{

		Json::Value node;

		node["path"] = component.getPath();

		for (auto& item : component.scriptClass.Attributes()) {
			node["attributes"].append(ToJson(item));
		}

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Script::ClassName()] = node;
	}

	void SceneArchiveOutput::Finalize() {
		Json::Value root;
		for (const auto & idx : m_Root) {
			if (!idx.isNull()) {
				root.append(idx);
			}
		}
		m_Root = root;
	}

	std::string SceneArchiveOutput::ToString()
	{
		std::stringstream output;
		output << m_Root;
		return output.str();
	}

	std::ostream &operator<<(std::ostream &stream, const SceneArchiveOutput& archiveOutput)
	{
		stream << archiveOutput.m_Root;
		return stream;
	}

	template<typename... Cpn>
	static SceneArchiveOutput SerializeComponents(ComponentGroup<Cpn...>, entt::registry& src)
	{
		SceneArchiveOutput archiveOutput;
		entt::snapshot snapshot{src};
		snapshot.component<Cpn...>(archiveOutput);

		archiveOutput.Finalize();
		return std::move(archiveOutput);
	}


	bool Serialization::SceneSerializer::Serialize(const std::filesystem::path &path)
	{
		std::ofstream file(path);
		if (!file.is_open())
			return false;

		file << SerializeComponents(AllComponents{}, m_Scene.GetRegistry());
		return true;
	}



	bool SceneSerializer::Serialize( Json::Value &root)
	{
		auto archive = SerializeComponents(AllComponents{}, m_Scene.GetRegistry());
		root = archive.m_Root;
		return true;
	}

	bool Serialization::SceneSerializer::Deserialize(Renderer& renderer, const std::filesystem::path &path)
	{
		Json::Value root;
		std::ifstream file(path);

		std::string err;
		const Json::CharReaderBuilder builder;
		if (!file.is_open() || !Json::parseFromStream(builder, file, &root, &err))
		{
			Log::coreError("Could not load Scene {0} : {1}", path.string(), err);
			return false;
		}

		return Deserialize(renderer, root);
	}

	bool SceneSerializer::Deserialize(Renderer& renderer, const Json::Value &root)
	{
		for (const auto& node: root)
		{
			std::string uuid = node[Component::ID::ClassName()].asString();
			UUID id;
			Entity entity;
			try {
				id = UUID::fromString(uuid);
				entity = m_Scene.CreateEntityFromId(id);
			}
			catch (const std::invalid_argument& e) {
				Log::warn("Entity with invalid UUID found");
				entity = m_Scene.CreateEntity();
			}


			auto& name = entity.getComponent<Component::Name>();
			name.name = node[Component::Name::ClassName()].asString();

			auto& component = entity.getComponent<Component::Transform>();
			component.position = node[Component::Transform::ClassName()]["position"].as<glm::vec3>();
			component.rotation = node[Component::Transform::ClassName()]["rotation"].as<glm::vec3>();
			component.scale = node[Component::Transform::ClassName()]["scale"].as<glm::vec3>();


			auto currComponent = Component::SpriteRenderer::ClassName();
			if (node.isMember(currComponent))
			{
				auto& spriteRenderer = entity.addComponent<Component::SpriteRenderer>();
				spriteRenderer.color = node[currComponent]["color"].as<glm::vec4>();

				auto texture = AssetsManager::Load<Texture2D>(node[currComponent]["texture"].asString());
				// if (texture && texture->IsLoaded())
				// {
				// 	spriteRenderer.texture = std::move(texture);
				// }

				spriteRenderer.tillingFactor = node[currComponent]["tillingFactor"].asFloat();
				spriteRenderer.zIndex = node[currComponent]["zIndex"].asInt();
			}

			currComponent = Component::MeshRendererC::ClassName();
			if (node.isMember(currComponent)) {
				auto& meshRenderer = entity.addComponent<Component::MeshRendererC>();

				auto mesh = AssetsManager::Load<Mesh>(node[currComponent]["path"].asString());
				meshRenderer.meshRenderer->SubmitMesh(renderer.GetDevice(), mesh);

				auto& materials = node[currComponent]["materials"];
				for (const auto& texturePath : materials) {
					auto texture = AssetsManager::Load<Texture2D>(texturePath.asString());
					// if (texture && texture->IsLoaded())
					// {
					// 	meshRenderer.meshRenderer->GetMesh()->GetMaterials().push_back(std::move(texture));
					// }
				}
			}

			currComponent = Component::Camera::ClassName();
			if (node.isMember(currComponent))
			{
				auto& camera = entity.addComponent<Component::Camera>();

				auto pFov = node[currComponent]["perspectiveFov"].asFloat();
				auto pNear = node[currComponent]["perspectiveNear"].asFloat();
				auto pFar = node[currComponent]["perspectiveFar"].asFloat();
				camera.camera.SetPerspective(pFov, pNear, pFar);


				auto oSize = node[currComponent]["orthoSize"].asFloat();
				auto oNear = node[currComponent]["orthoNear"].asFloat();
				auto oFar = node[currComponent]["orthoFar"].asFloat();
				camera.camera.SetOrthographic(oSize, oNear, oFar);

				camera.camera.SetProjectionType(
					static_cast<SceneCamera::ProjectionType>(node["camera"]["type"].asInt()));
			}


			currComponent = Component::Light::ClassName();
			if (node.isMember(currComponent))
			{
				auto& light = entity.addComponent<Component::Light>();
				light.lightType = node[currComponent]["type"].asInt();
				light.color = node[currComponent]["color"].as<glm::vec3>();

				light.range = node[currComponent]["range"].asFloat();
				light.angle = node[currComponent]["angle"].asFloat();
				light.smoothness = node[currComponent]["smoothness"].asFloat();

			}

			currComponent = Component::Script::ClassName();
			if (node.isMember(currComponent))
			{
				auto& behavior = entity.addComponent<Component::Script>();
				behavior.setPath(node[currComponent]["path"].asString());

				auto& attrsNode = node[currComponent]["attributes"];
				auto& attrs = behavior.scriptClass.Attributes();
				for (const auto& subNode: attrsNode) {
					auto field = subNode.as<ScriptField>();
					auto attrPair = std::find_if(attrs.begin(), attrs.end(), [&](auto& attr) {
						return attr == field;
					});
					if (attrPair != attrs.end()) {
						*attrPair = field;
					}
				}
			}
		}
		return true;
	}
}
