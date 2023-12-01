#pragma once

#include "Cardia/Core/Concepts.hpp"
#include "Cardia/Serialization/Serializable.hpp"


namespace Cardia
{
	template <arithmetic T>
	struct Vector4;

	template<arithmetic T>
	struct Vector3
	{
		union { T x, r; };
		union { T y, g; };
		union { T z, b; };

		Vector3() = default;

		constexpr Vector3(T x, T y, T z) noexcept
			: x(x), y(y), z(z) {}

		explicit constexpr Vector3(T scalar) noexcept
			: x(scalar), y(scalar), z(scalar) {}
		
		explicit constexpr Vector3(const Vector4<T>& other) noexcept;
		constexpr Vector3& operator=(const Vector4<T>& other) noexcept;

		constexpr bool operator==(const Vector3& other) const noexcept;
		constexpr bool operator!=(const Vector3& other) const noexcept;

		static constexpr size_t Size() noexcept { return 3; }

		constexpr T& operator[](size_t index) noexcept;
		constexpr T operator[](size_t index) const noexcept;

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

		constexpr T Length() const noexcept;
		constexpr T Dot(const Vector3& other) const noexcept;
		constexpr Vector3 Lerp(const Vector3& other, float t) const noexcept;
		constexpr Vector3 Cross(const Vector3& other) const noexcept;
		constexpr Vector3 Normalize() const noexcept;

		constexpr static auto properties = std::make_tuple(
			property(&Vector3::x, "x"),
			property(&Vector3::y, "y"),
			property(&Vector3::z, "z")
		);
	};

	using Vector3f = Vector3<float>;
	using Vector3i = Vector3<int32_t>;
	using Vector3u = Vector3<uint32_t>;
}

#include "Vector3.inl"
