#pragma once
#include <json/value.h>
#include <optional>

namespace Cardia
{

	template<typename Class, typename T>
	struct PropertyImpl {
		constexpr PropertyImpl(T Class::* aMember, const char* aName)
			: member{aMember}, name{aName} {}
		constexpr PropertyImpl(T (Class::* aGetter)() const, void (Class::* aSetter)(const T&), const char* aName)
			: name{aName}, getter{aGetter}, setter{aSetter}, isGetSet{true} { }

		using Type = T;

		T Class::*member = nullptr;
		const char* name = nullptr;


		T (Class::*getter)() const = nullptr;
		void (Class::*setter)(const T&) = nullptr;

		const bool isGetSet = false;
	};

	template<typename Class, typename T>
	constexpr auto property(T Class::*member, const char* name) {
		return PropertyImpl<Class, T>{member, name};
	}

	template<typename Class, typename T>
	constexpr auto property(T (Class::*getter)() const, void (Class::*setter)(const T&), const char* name) {
		return PropertyImpl<Class, T>{getter, setter, name};
	}
}
