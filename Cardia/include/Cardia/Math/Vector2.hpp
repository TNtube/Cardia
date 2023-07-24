#pragma once

#include <json/value.h>

#include "Cardia/Core/Concepts.hpp"


namespace Cardia
{

	template<arithmetic T>
	struct Vector2
	{
		union { T x, r; };
		union { T y, g; };

		Vector2() = default;

		Vector2(T x, T y)
			: x(x), y(y) {}

		explicit Vector2(T scalar)
			: x(scalar), y(scalar) {}

		static constexpr size_t length() { return 2; }

		T& operator[](size_t index);

		Vector2 operator-() const;

		Vector2& operator+=(const Vector2& other);
		Vector2 operator+(const Vector2& other) const;
		Vector2& operator-=(const Vector2& other);
		Vector2 operator-(const Vector2& other) const;
		Vector2& operator*=(const Vector2& other);
		Vector2 operator*(const Vector2& other) const;
		Vector2& operator/=(const Vector2& other);
		Vector2 operator/(const Vector2& other) const;

		template<typename U>
		Vector2& operator+=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector2 operator+(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector2& operator-=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector2 operator-(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector2& operator*=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector2 operator*(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector2& operator/=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector2 operator/(U scalar) const requires std::convertible_to<U, T>;

		T Dot(const Vector2& other);
		Vector2 Lerp(const Vector2& other, float t);

		Json::Value Serialize() const;
		static bool Deserialize(const Json::Value& root, Vector2& other);
	};

	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int32_t>;
	using Vector2u = Vector2<uint32_t>;
}

#include "Vector2.inl"
