#pragma once
#include <string>
#include <uuid.h>

#include "Cardia/Serialization/Serializer.hpp"
#include <json/value.h>

namespace Cardia {

	class UUID
	{
	public:
		UUID();
		explicit UUID(const uuids::uuid& uuid);
		UUID(UUID&& other) noexcept;
		UUID& operator=(UUID&& other) noexcept;
		UUID(const UUID&) = default;
		UUID& operator=(const UUID&) = default;

		static UUID Default() { ;return UUID(uuids::uuid()); };
		static UUID FromString(const std::string& strUuid);
		std::string ToString() const { return uuids::to_string(m_UUID); }

		bool IsValid() const;

		std::size_t Hash() const { return std::hash<uuids::uuid>{}(m_UUID); }

		inline bool operator==(const UUID& rhs) const { return this->m_UUID == rhs.m_UUID; }
	private:
		std::array<uint8_t, 16> m_UUID{};
		friend std::hash<UUID>;

	public:
		constexpr static auto properties = std::make_tuple(
			property(&UUID::m_UUID, "uuid")
		);
	};

}

template<>
struct std::hash<Cardia::UUID>
{
	std::size_t operator()(const Cardia::UUID& uuid) const noexcept
	{
		static auto h = std::hash<uuids::uuid>{};
		return h(uuid.m_UUID);
	}
};
