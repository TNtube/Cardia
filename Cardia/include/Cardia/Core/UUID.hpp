#pragma once
#include <string>

#include "Cardia/Serialization/Serializer.hpp"

namespace uuids
{
	class uuid;
}

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

		static UUID Default();
		static UUID FromString(const std::string& strUuid);
		std::string ToString() const;

		bool IsValid() const;

		std::size_t Hash() const;

		inline bool operator==(const UUID& rhs) const { return this->m_UUID == rhs.m_UUID; }
	private:
		void InternalFromString(const std::string& strUuid);
		std::array<uint8_t, 16> m_UUID{};
		friend std::hash<UUID>;

	public:
		constexpr static auto properties = std::make_tuple(
			property(&UUID::ToString, &UUID::InternalFromString, "uuid")
		);
	};

}

template<>
struct std::hash<Cardia::UUID>
{
	std::size_t operator()(const Cardia::UUID& uuid) const noexcept
	{
		return uuid.Hash();
	}
};
