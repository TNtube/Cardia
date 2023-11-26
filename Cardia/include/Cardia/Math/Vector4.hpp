#pragma once

#include <json/value.h>

#include "Cardia/Serialization/Serializable.hpp"
#include "Cardia/Core/Concepts.hpp"
#include "Vector3.hpp"


namespace Cardia
{

	template<arithmetic T>
	struct Vector4
	{
		union { T x, r; };
		union { T y, g; };
		union { T z, b; };
		union { T w, a; };

		Vector4() = default;

		constexpr Vector4(T x, T y, T z, T w) noexcept
			: x(x), y(y), z(z), w(w) {}

		constexpr Vector4(const Vector3<T>& other, T w) noexcept
			: x(other.x), y(other.y), z(other.z), w(w) {}

		explicit constexpr Vector4(T scalar) noexcept
			: x(scalar), y(scalar), z(scalar), w(scalar) {}

		constexpr bool operator==(const Vector4& other) const noexcept;
		constexpr bool operator!=(const Vector4& other) const noexcept;

		static constexpr size_t Size() noexcept { return 4; }

		constexpr T operator[](size_t index) const noexcept;
		constexpr T& operator[](size_t index) noexcept;

		constexpr Vector4 operator-() const noexcept;

		constexpr Vector4& operator+=(const Vector4& other) noexcept;
		constexpr Vector4 operator+(const Vector4& other) const noexcept;
		constexpr Vector4& operator-=(const Vector4& other) noexcept;
		constexpr Vector4 operator-(const Vector4& other) const noexcept;
		constexpr Vector4& operator*=(const Vector4& other) noexcept;
		constexpr Vector4 operator*(const Vector4& other) const noexcept;
		constexpr Vector4& operator/=(const Vector4& other) noexcept;
		constexpr Vector4 operator/(const Vector4& other) const noexcept;

		template<typename U>
		constexpr Vector4& operator+=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector4 operator+(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector4& operator-=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector4 operator-(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector4& operator*=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector4 operator*(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector4& operator/=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector4 operator/(U scalar) const noexcept requires std::convertible_to<U, T>;

		constexpr T Length() noexcept;
		constexpr T Dot(const Vector4& other) noexcept;
		constexpr Vector4 Lerp(const Vector4& other, float t) noexcept;
		constexpr Vector4 Normalize() noexcept;

		constexpr static auto properties = std::make_tuple(
			property(&Vector4::x, "x"),
			property(&Vector4::y, "y"),
			property(&Vector4::z, "z"),
			property(&Vector4::w, "w")
		);
	};

	using Vector4f = Vector4<float>;
	using Vector4i = Vector4<int32_t>;
	using Vector4u = Vector4<uint32_t>;
}

#include "Vector4.inl"
