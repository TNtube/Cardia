#pragma once

#include <json/value.h>

#include "Cardia/Core/Concepts.hpp"


namespace Cardia
{

	template<arithmetic T>
	struct Vector3
	{
		union { T x, r; };
		union { T y, g; };
		union { T z, a; };

		Vector3() = default;

		constexpr Vector3(T x, T y, T z) noexcept
			: x(x), y(y), z(z) {}

		explicit constexpr Vector3(T scalar) noexcept
			: x(scalar), y(scalar), z(scalar) {}

		static constexpr size_t size() noexcept { return 3; }

		constexpr T& operator[](size_t index) noexcept;

		constexpr Vector3 operator-() const noexcept;

		constexpr Vector3& operator+=(const Vector3& other) noexcept;
		constexpr Vector3 operator+(const Vector3& other) const noexcept;
		constexpr Vector3& operator-=(const Vector3& other) noexcept;
		constexpr Vector3 operator-(const Vector3& other) const noexcept;
		constexpr Vector3& operator*=(const Vector3& other) noexcept;
		constexpr Vector3 operator*(const Vector3& other) const noexcept;
		constexpr Vector3& operator/=(const Vector3& other) noexcept;
		constexpr Vector3 operator/(const Vector3& other) const noexcept;

		template<typename U>
		constexpr Vector3& operator+=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector3 operator+(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector3& operator-=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector3 operator-(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector3& operator*=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector3 operator*(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector3& operator/=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector3 operator/(U scalar) const noexcept requires std::convertible_to<U, T>;

		constexpr T Dot(const Vector3& other) noexcept;
		constexpr Vector3 Lerp(const Vector3& other, float t) noexcept;
		constexpr Vector3 Cross(const Vector3& other) noexcept;
		constexpr Vector3 Normalize() noexcept;

		Json::Value Serialize() const;
		static bool Deserialize(const Json::Value& root, Vector3& other);
	};

	using Vector3f = Vector3<float>;
	using Vector3i = Vector3<int32_t>;
	using Vector3u = Vector3<uint32_t>;
}

#include "Vector3.inl"
