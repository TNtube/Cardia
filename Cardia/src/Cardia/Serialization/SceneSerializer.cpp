#include "cdpch.hpp"

#include "Cardia/ECS/Components.hpp"
#include <json/json.h>
#include <Cardia/Serialization/SceneSerializer.hpp>
#include <Cardia/Project/Project.hpp>


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

	bool Serialization::SceneSerializer::Deserialize(const std::filesystem::path &path)
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

		for (const auto& uuid: root.getMemberNames())
		{
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

			auto& node = root[uuid];
			auto& name = entity.getComponent<Component::Name>();
			name.name = node["name"].asString();

			auto& component = entity.getComponent<Component::Transform>();
			component.position = node["transform"]["position"].as<glm::vec3>();
			component.rotation = node["transform"]["rotation"].as<glm::vec3>();
			component.scale = node["transform"]["scale"].as<glm::vec3>();

			if (node.isMember("spriteRenderer"))
			{
				auto& spriteRenderer = entity.addComponent<Component::SpriteRenderer>();
				spriteRenderer.color = node["spriteRenderer"]["color"].as<glm::vec4>();

				const auto& assetsPath = Project::GetAssetDirectory();
				auto texture = Texture2D::create((assetsPath / node["spriteRenderer"]["texture"].asString()).string());
				if (texture->isLoaded())
				{
					spriteRenderer.texture = std::move(texture);
				}

				spriteRenderer.tillingFactor = node["spriteRenderer"]["tillingFactor"].asFloat();
				spriteRenderer.zIndex = node["spriteRenderer"]["zIndex"].asInt();
			}

			if (node.isMember("meshRenderer")) {
				auto& meshRenderer = entity.addComponent<Component::MeshRendererC>();

				const auto& assetsPath = Project::GetAssetDirectory();
				auto texture = Texture2D::create((assetsPath / node["meshRenderer"]["texture"].asString()).string());
				if (texture->isLoaded())
				{
					meshRenderer.texture = std::move(texture);
				}

				meshRenderer.path = node["meshRenderer"]["path"].asString();
				auto mesh = Mesh::ReadMeshFromFile((assetsPath / meshRenderer.path).string());
				meshRenderer.meshRenderer->SubmitMesh(mesh);

			}

			if (node.isMember("camera"))
			{
				auto& camera = entity.addComponent<Component::Camera>();

				auto pFov = node["camera"]["perspectiveFov"].asFloat();
				auto pNear = node["camera"]["perspectiveNear"].asFloat();
				auto pFar = node["camera"]["perspectiveFar"].asFloat();
				camera.camera.SetPerspective(pFov, pNear, pFar);


				auto oSize = node["camera"]["orthoSize"].asFloat();
				auto oNear = node["camera"]["orthoNear"].asFloat();
				auto oFar = node["camera"]["orthoFar"].asFloat();
				camera.camera.SetOrthographic(oSize, oNear, oFar);

				camera.camera.SetProjectionType(
					static_cast<SceneCamera::ProjectionType>(node["camera"]["type"].asInt()));
			}

			// PointLight
			if (node.isMember("light"))
			{
				auto& light = entity.addComponent<Component::Light>();
				light.lightType = node["light"]["type"].asInt();
				light.color = node["light"]["color"].as<glm::vec3>();

				light.range = node["light"]["range"].asFloat();
				light.angle = node["light"]["angle"].asFloat();
				light.smoothness = node["light"]["smoothness"].asFloat();

			}

			if (node.isMember("behavior"))
			{
				auto& behavior = entity.addComponent<Component::Script>();
				behavior.setPath(node["behavior"]["path"].asString());

				auto& attrsNode = node["behavior"]["attributes"];
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

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Transform& component)
	{
		auto& uuid = m_Registry.get<Component::ID>(entity).uuid;
		Json::Value node;
		node["position"] = ToJson(component.position);
		node["rotation"] = ToJson(component.rotation);
		node["scale"] = ToJson(component.scale);
		m_Root[uuid]["transform"] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Name& component)
	{
		auto& uuid = m_Registry.get<Component::ID>(entity).uuid;
		m_Root[uuid]["name"] = component.name;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::MeshRendererC& component)
	{
		auto& uuid = m_Registry.get<Component::ID>(entity).uuid;
		Json::Value node;

		node["path"] = component.path;
		const auto texPath = component.texture ? component.texture->getPath() : "";
		node["texture"] = texPath;

		m_Root[uuid]["meshRenderer"] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::SpriteRenderer& component)
	{
		auto& uuid = m_Registry.get<Component::ID>(entity).uuid;
		Json::Value node;

		node["color"] = ToJson(component.color);
		const auto texPath = component.texture ? component.texture->getPath() : "";

		node["texture"] = texPath;
		node["tillingFactor"] = component.tillingFactor;
		node["zIndex"] = component.zIndex;

		m_Root[uuid]["spriteRenderer"] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Camera &component)
	{
		auto& uuid = m_Registry.get<Component::ID>(entity).uuid;
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

		m_Root[uuid]["camera"] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Light &component)
	{
		auto& uuid = m_Registry.get<Component::ID>(entity).uuid;
		Json::Value node;

		node["type"] = component.lightType;
		node["color"] = ToJson(component.color);

		node["range"] = component.range;
		node["angle"] = component.angle;
		node["smoothness"] = component.smoothness;

		m_Root[uuid]["light"] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Script &component)
	{
		auto& uuid = m_Registry.get<Component::ID>(entity).uuid;
		Json::Value node;

		node["path"] = component.getPath();

		for (auto& item : component.scriptClass.Attributes()) {
			node["attributes"].append(ToJson(item));
		}

		m_Root[uuid]["behavior"] = node;
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


	template<typename... Component>
	static SceneArchiveOutput SerializeComponents(ComponentGroup<Component...>, entt::registry& src) {

		SceneArchiveOutput archiveOutput(src);
		entt::snapshot snapshot{src};
		(snapshot.component<Component>(archiveOutput), ...);
		Log::coreInfo("{0}", archiveOutput.ToString());
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



	void SceneArchiveInput::operator()(entt::entity &entity, const Component::Transform &component)
	{

	}

	void SceneArchiveInput::operator()(entt::entity &entity, const Component::Name &component)
	{

	}

	void SceneArchiveInput::operator()(entt::entity &entity, const Component::MeshRendererC &component)
	{

	}

	void SceneArchiveInput::operator()(entt::entity &entity, const Component::SpriteRenderer &component)
	{

	}

	void SceneArchiveInput::operator()(entt::entity &entity, const Component::Camera &component)
	{

	}

	void SceneArchiveInput::operator()(entt::entity &entity, const Component::Light &component)
	{

	}

	void SceneArchiveInput::operator()(entt::entity &entity, const Component::Script &component)
	{

	}

	std::unique_ptr<Scene> SceneArchiveInput::ToScene()
	{
		return std::make_unique<Scene>();
	}
}
