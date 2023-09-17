#pragma once
#include <filesystem>
#include <utility>
#include "Cardia/Core/UUID.hpp"


namespace Cardia
{
	struct AssetHandle
	{
		UUID ID;

		static AssetHandle Invalid() { return { UUID::Default() }; }

		bool IsValid() const { return ID.IsValid(); }

		bool operator==(const AssetHandle& other) const { return ID == other.ID; }

		Json::Value Serialize() const {
			Json::Value value;
			value["ID"] = ID.ToString();
		}
		static std::optional<AssetHandle> Deserialize(const Json::Value& value) {
			if (value["ID"].isNull())
				return {};
			return AssetHandle{UUID::FromString(value["ID"].asString())};
		}

	};

	class Asset
	{
	public:
		explicit Asset(AssetHandle assetHandle) : m_Handle(std::move(assetHandle)) {};
		Asset() = default;
		virtual ~Asset() = default;

		const AssetHandle& GetHandle() const { return m_Handle; }

		virtual void Reload() = 0;

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