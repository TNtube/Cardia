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
		union { T z, b; };

		Vector3() = default;

		Vector3(T x, T y, T z)
			: x(x), y(y), z(z) {}

		explicit Vector3(T scalar)
			: x(scalar), y(scalar), z(scalar) {}
		Vector3 operator-() const;

		Vector3& operator+=(const Vector3& other);
		Vector3 operator+(const Vector3& other) const;
		Vector3& operator-=(const Vector3& other);
		Vector3 operator-(const Vector3& other) const;
		Vector3& operator*=(const Vector3& other);
		Vector3 operator*(const Vector3& other) const;
		Vector3& operator/=(const Vector3& other);
		Vector3 operator/(const Vector3& other) const;

		template<typename U>
		Vector3& operator+=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector3 operator+(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector3& operator-=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector3 operator-(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector3& operator*=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector3 operator*(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector3& operator/=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector3 operator/(U scalar) const requires std::convertible_to<U, T>;

		T Dot(const Vector3& other);
		Vector3 Lerp(const Vector3& other, float t);
		Vector3 Cross(const Vector3& other);

		Json::Value Serialize() const;
		static bool Deserialize(const Json::Value& root, Vector3& other);
	};

	using Vector3f = Vector3<float>;
	using Vector3i = Vector3<int32_t>;
	using Vector3u = Vector3<uint32_t>;
}

#include "Vector3.inl"
