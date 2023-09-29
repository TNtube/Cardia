#pragma once
#include <string>
#include <uuid.h>

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

		static UUID Default() { return UUID(uuids::uuid()); };
		static UUID FromString(const std::string& strUuid);
		std::string ToString() const { return uuids::to_string(m_UUID); }

		bool IsValid() const { return !m_UUID.is_nil(); }

		std::size_t Hash() const { return std::hash<uuids::uuid>{}(m_UUID); }

		inline bool operator==(const UUID& rhs) const { return this->m_UUID == rhs.m_UUID; }
	private:
		uuids::uuid m_UUID;
		friend std::hash<UUID>;
	};

}

namespace std {
	template<>
	struct hash<Cardia::UUID>
	{
		std::size_t operator()(const Cardia::UUID& uuid) const
		{
			static auto h = std::hash<uuids::uuid>{};
			return h(uuid.m_UUID);
		}
	};

}
