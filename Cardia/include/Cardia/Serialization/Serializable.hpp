#pragma once
#include <json/value.h>
#include <optional>

namespace Cardia
{

	template<typename Class, typename T>
	struct PropertyImpl {
		constexpr PropertyImpl(T Class::*aMember, const char* aName) : member{aMember}, name{aName} {}

		using Type = T;

		T Class::*member;
		const char* name;
	};

	template<typename Class, typename T>
	constexpr auto property(T Class::*member, const char* name) {
		return PropertyImpl<Class, T>{member, name};
	}
}
