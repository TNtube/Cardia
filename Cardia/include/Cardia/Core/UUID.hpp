#pragma once
#include <string>
#include <uuid.h>

#include "Cardia/Serialization/Serializable.hpp"

namespace Cardia {

	class UUID : public std::string
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
		std::string ToString() const { return m_UUID; }

		bool IsValid() const { return !uuids::uuid::is_valid_uuid(m_UUID); }

		std::size_t Hash() const { return std::hash<std::string>{}(m_UUID); }

		inline bool operator==(const UUID& rhs) const { return this->m_UUID == rhs.m_UUID; }
	private:
		std::string m_UUID;
		friend std::hash<UUID>;

	public:
		constexpr static auto properties = std::make_tuple(
			property(&UUID::m_UUID, "UUID")
		);
	};

}

namespace std {
	template<>
	struct hash<Cardia::UUID>
	{
		std::size_t operator()(const Cardia::UUID& uuid) const noexcept
		{
			static auto h = std::hash<std::string>{};
			return h(uuid.m_UUID);
		}
	};

}
