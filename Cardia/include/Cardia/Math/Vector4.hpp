#pragma once

#include <json/value.h>

#include "Cardia/Core/Concepts.hpp"


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

		Vector4(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w) {}

		explicit Vector4(T scalar)
			: x(scalar), y(scalar), z(scalar), w(w) {}

		static constexpr size_t length() { return 4; }

		T& operator[](size_t index);

		Vector4 operator-() const;

		Vector4& operator+=(const Vector4& other);
		Vector4 operator+(const Vector4& other) const;
		Vector4& operator-=(const Vector4& other);
		Vector4 operator-(const Vector4& other) const;
		Vector4& operator*=(const Vector4& other);
		Vector4 operator*(const Vector4& other) const;
		Vector4& operator/=(const Vector4& other);
		Vector4 operator/(const Vector4& other) const;

		template<typename U>
		Vector4& operator+=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector4 operator+(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector4& operator-=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector4 operator-(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector4& operator*=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector4 operator*(U scalar) const requires std::convertible_to<U, T>;
		template<typename U>
		Vector4& operator/=(U scalar) requires std::convertible_to<U, T>;
		template<typename U>
		Vector4 operator/(U scalar) const requires std::convertible_to<U, T>;

		T Dot(const Vector4& other);
		Vector4 Lerp(const Vector4& other, float t);

		Json::Value Serialize() const;
		static bool Deserialize(const Json::Value& root, Vector4& other);
	};

	using Vector4f = Vector4<float>;
	using Vector4i = Vector4<int32_t>;
	using Vector4u = Vector4<uint32_t>;
}

#include "Vector4.inl"
