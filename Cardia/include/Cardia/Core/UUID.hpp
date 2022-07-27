#pragma once
#include <string>
#include <uuid.h>

namespace Cardia {

	class UUID
	{
	public:
		UUID();
		UUID(const uuids::uuid& uuid);
		UUID(const std::string& strUuid);
		UUID(const UUID&) = default;

		operator std::string() const { return to_string(m_UUID); }
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
