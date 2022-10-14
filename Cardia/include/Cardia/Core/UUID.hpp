#pragma once
#include <string>
#include <uuid.h>

namespace Cardia {

	class UUID
	{
	public:
		UUID();
		UUID(const uuids::uuid& uuid);
		UUID(const UUID&) = default;

		static UUID fromString(const std::string& strUuid);

		operator std::string() const { return to_string(m_UUID); }
		inline bool operator==(const UUID& rhs) const { return this->m_UUID == rhs.m_UUID; };
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
