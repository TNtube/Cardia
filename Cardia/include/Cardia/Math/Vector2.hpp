#pragma once

#include "Cardia/Core/Concepts.hpp"


namespace Cardia
{
	template <arithmetic T>
	struct Vector4;
	template <arithmetic T>
	struct Vector3;

	template<arithmetic T>
	struct Vector2
	{
		union { T x, r, w; };
		union { T y, g, h; };

		Vector2() = default;

		constexpr Vector2(T x, T y) noexcept
			: x(x), y(y) {}

		explicit constexpr Vector2(T scalar) noexcept
			: x(scalar), y(scalar) {}

		explicit constexpr Vector2(const Vector3<T>& other) noexcept;
		constexpr Vector2& operator=(const Vector3<T>& other) noexcept;
		explicit constexpr Vector2(const Vector4<T>& other) noexcept;
		constexpr Vector2& operator=(const Vector4<T>& other) noexcept;

		static constexpr size_t Size() noexcept { return 2; }

		constexpr T& operator[](size_t index) noexcept;

		constexpr Vector2 operator-() const noexcept;

		constexpr bool operator==(const Vector2& other) const noexcept;
		constexpr bool operator!=(const Vector2& other) const noexcept;

		constexpr Vector2& operator+=(const Vector2& other) noexcept;
		constexpr Vector2 operator+(const Vector2& other) const noexcept;
		constexpr Vector2& operator-=(const Vector2& other) noexcept;
		constexpr Vector2 operator-(const Vector2& other) const noexcept;
		constexpr Vector2& operator*=(const Vector2& other) noexcept;
		constexpr Vector2 operator*(const Vector2& other) const noexcept;
		constexpr Vector2& operator/=(const Vector2& other) noexcept;
		constexpr Vector2 operator/(const Vector2& other) const noexcept;

		template<typename U>
		constexpr Vector2& operator+=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector2 operator+(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector2& operator-=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector2 operator-(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector2& operator*=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector2 operator*(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector2& operator/=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Vector2 operator/(U scalar) const noexcept requires std::convertible_to<U, T>;

		constexpr T Length() noexcept;
		constexpr T Dot(const Vector2& other) noexcept;
		constexpr Vector2 Lerp(const Vector2& other, float t) noexcept;
		constexpr Vector2 Normalize() noexcept;

		static constexpr Vector2 Zero() noexcept;

		constexpr static auto properties = std::make_tuple(
			property(&Vector2::x, "x"),
			property(&Vector2::y, "y")
		);
	};

	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int32_t>;
	using Vector2u = Vector2<uint32_t>;
}

#include "Vector2.inl"
