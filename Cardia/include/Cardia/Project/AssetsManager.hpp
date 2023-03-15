#pragma once

#include <memory>
#include <Cardia/Core/Core.hpp>
#include <string>
#include <Cardia/Renderer/Shader.hpp>
#include <typeindex>
#include <Cardia/Renderer/Texture.hpp>
#include <Cardia/DataStructure/Mesh.hpp>
#include <utility>

namespace {
	struct TypeID
	{
		std::type_index type_index;
		std::string ID;
		bool operator==(const TypeID& other) const {
			return ID == other.ID && type_index == other.type_index;
		}
	};

	struct AssetRefCounter
	{
		AssetRefCounter() = default;
		explicit AssetRefCounter(std::shared_ptr<void> resource) : Resource(std::move(resource)) {}
		std::shared_ptr<void> Resource;
		uint32_t UnusedCounter = 0;
	};
}

namespace std {
	template<>
	struct hash<TypeID> {
		auto operator()(const TypeID& typeId) const -> ::size_t {
			return hash<std::string>{}(typeId.ID) ^ hash<std::type_index>{}(typeId.type_index);
		}
	};
}

namespace Cardia
{
	class AssetsManager
	{
	public:
		AssetsManager();
		~AssetsManager();
		template<typename T>
		static std::shared_ptr<T> Load(const std::filesystem::path& path);

		static void CollectGarbage(bool forceCollection = true);

	private:
		std::unordered_map<TypeID, AssetRefCounter> m_Assets;

		// Threading related
		void CollectThread();
		std::thread m_CollectThread;
		std::mutex m_CounterMutex;
		std::condition_variable m_CollectCV;
		bool m_ShouldStopCollecting = false;

		static std::shared_ptr<AssetsManager> s_ActiveManager;
	};

	template<typename T>
	inline std::shared_ptr<T> AssetsManager::Load(const std::filesystem::path& path)
	{
		cdCoreAssert(false, std::format("Unknown assets type {}", typeid(T).name()));
		return std::shared_ptr<T>();
	}

	template<>
	inline std::shared_ptr<Shader> AssetsManager::Load(const std::filesystem::path& path)
	{
		TypeID id {typeid(Shader), path.string()};

		if (!s_ActiveManager->m_Assets.contains(id)) {
			AssetRefCounter res(Shader::create({path.string() + ".vert", path.string() + ".frag"}));
			s_ActiveManager->m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Shader>(s_ActiveManager->m_Assets[id].Resource);
	}

	template<>
	inline std::shared_ptr<Texture2D> AssetsManager::Load(const std::filesystem::path& path)
	{
		TypeID id {typeid(Texture2D), path.string()};

		if (!s_ActiveManager->m_Assets.contains(id)) {
			AssetRefCounter res(Texture2D::create(path.string()));
			s_ActiveManager->m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Texture2D>(s_ActiveManager->m_Assets[id].Resource);
	}

	template<>
	inline std::shared_ptr<Mesh> AssetsManager::Load(const std::filesystem::path& path)
	{
		TypeID id {typeid(Mesh), path.string()};

		if (!s_ActiveManager->m_Assets.contains(id)) {
			AssetRefCounter res(std::make_unique<Mesh>(Mesh::ReadMeshFromFile(path.string())));
			s_ActiveManager->m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Mesh>(s_ActiveManager->m_Assets[id].Resource);

	}
}