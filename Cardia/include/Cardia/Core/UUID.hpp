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

		static UUID FromString(const std::string& strUuid);
		std::string ToString() const { return uuids::to_string(m_UUID); }

		bool IsValid() const { return !m_UUID.is_nil(); }

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
