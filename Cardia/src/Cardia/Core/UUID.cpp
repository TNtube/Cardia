#include "cdpch.hpp"
#include "Cardia/Core/UUID.hpp"
#include <algorithm>
#include <uuid.h>

namespace Cardia
{

	static std::random_device rd;
	static std::mt19937 gen(rd());
	static uuids::uuid_random_generator uuidRandomGenerator(gen);

	UUID::UUID(): UUID(uuidRandomGenerator()) {}

	UUID::UUID(const uuids::uuid& uuid)
	{
		const auto span = uuid.as_bytes();
		const auto begin = reinterpret_cast<const uint8_t*>(span.data());
		std::copy_n(begin, span.size(), m_UUID.begin());
	}

	UUID::UUID(UUID&& other) noexcept
	{
		m_UUID.swap(other.m_UUID);
	}

	UUID& UUID::operator=(UUID&& other) noexcept
	{
		m_UUID.swap(other.m_UUID);
		return *this;
	}

	UUID UUID::Default()
	{
		return UUID(uuids::uuid());
	}

	UUID UUID::FromString(const std::string &uuid) {
		UUID _uuid;
		_uuid.InternalFromString(uuid);
		return _uuid;
	}

	std::string UUID::ToString() const
	{
		return uuids::to_string(m_UUID);
	}

	bool UUID::IsValid() const
	{
		for (const std::uint8_t i : m_UUID)
		{
			if (i != 0) return false;
		}
		return true;
	}

	std::size_t UUID::Hash() const
	{
		static auto h = std::hash<uuids::uuid>{};
		return h(m_UUID);
	}

	void UUID::InternalFromString(const std::string& strUuid)
	{
		const auto id = uuids::uuid::from_string(strUuid);
		if (!id.has_value()) {
			throw std::invalid_argument("UUID : " + strUuid + " is not a valid id");
		}
		const auto span = id->as_bytes();
		const auto begin = reinterpret_cast<const uint8_t*>(span.data());
		std::copy_n(begin, span.size(), m_UUID.begin());
	}
}
