#pragma once
#include <filesystem>
#include <utility>
#include <json/value.h>

#include "Cardia/Core/UUID.hpp"
#include "Cardia/Serialization/Serializable.hpp"


namespace Cardia
{
	struct AssetHandle
	{
		UUID ID;

		static AssetHandle Invalid() { return { UUID::Default() }; }

		bool IsValid() const { return ID.IsValid(); }

		bool operator==(const AssetHandle& other) const { return ID == other.ID; }

		constexpr static auto properties = std::make_tuple(
			property(&AssetHandle::ID, "ID")
		);

	};

	class Asset
	{
	public:
		explicit Asset(AssetHandle assetHandle) : m_Handle(std::move(assetHandle)) {};
		Asset() = default;
		virtual ~Asset() = default;

		const AssetHandle& GetHandle() const { return m_Handle; }

		virtual void Reload() = 0;
		virtual bool CheckForDirtyInDependencies() = 0; // return false if any asset dependency is invalid

	protected:
		AssetHandle m_Handle;
	};
}

namespace std {
	template<>
	struct hash<Cardia::AssetHandle> {
		auto operator()(const Cardia::AssetHandle& handle) const noexcept -> ::size_t {
			return handle.ID.Hash();
		}
	};
}