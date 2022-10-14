#include "cdpch.hpp"
#include "Cardia/Core/UUID.hpp"
#include <algorithm>

namespace Cardia
{

	static std::random_device rd;
	static std::mt19937 gen(rd());
	static uuids::uuid_random_generator uuidRandomGenerator(gen);

	UUID::UUID() {
		m_UUID = uuidRandomGenerator();
	}

	UUID::UUID(const uuids::uuid & uuid) {
		m_UUID = uuid;
	}

	UUID UUID::fromString(const std::string &uuid) {
		auto id =  uuids::uuid::from_string(uuid);
		if (!id.has_value()) {
			throw std::invalid_argument("UUID : " + uuid + " is not a valid id");
		}
		return id.value();
	}
}
