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

		Vector2& operator+=(const Vector2& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		Vector2 operator+(const Vector2& other) const
		{
			Vector2 temp(*this);
			temp += other;
			return temp;
		}

		template<typename U>
		Vector2& operator+=(U scalar) requires std::convertible_to<U, T>
		{
			x += static_cast<T>(scalar);
			y += static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector2 operator+(U scalar) const requires std::convertible_to<U, T>
		{
			Vector2 temp(*this);
			temp += static_cast<T>(scalar);
			return temp;
		}

		Vector2 operator-() const
		{
			return Vector2(-x, -y);
		}

		Vector2& operator-=(const Vector2& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vector2 operator-(const Vector2& other) const
		{
			Vector2 temp(*this);
			temp -= other;
			return temp;
		}

		template<typename U>
		Vector2& operator-=(U scalar) requires std::convertible_to<U, T>
		{
			x -= static_cast<T>(scalar);
			y -= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector2 operator-(U scalar) const requires std::convertible_to<U, T>
		{
			Vector2 temp(*this);
			temp -= static_cast<T>(scalar);
			return temp;
		}

		Vector2& operator*=(const Vector2& other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		Vector2 operator*(const Vector2& other) const
		{
			Vector2 temp(*this);
			temp *= other;
			return temp;
		}

		template<typename U>
		Vector2& operator*=(U scalar) requires std::convertible_to<U, T>
		{
			x *= static_cast<T>(scalar);
			y *= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector2 operator*(U scalar) const requires std::convertible_to<U, T>
		{
			Vector2 temp(*this);
			temp *= static_cast<T>(scalar);
			return temp;
		}

		Vector2& operator/=(const Vector2& other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}

		Vector2 operator/(const Vector2& other) const
		{
			Vector2 temp(*this);
			temp /= other;
			return temp;
		}

		template<typename U>
		Vector2& operator/=(U scalar) requires std::convertible_to<U, T>
		{
			x /= static_cast<T>(scalar);
			y /= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector2 operator/(U scalar) const requires std::convertible_to<U, T>
		{
			Vector2 temp(*this);
			temp /= static_cast<T>(scalar);
			return temp;
		}

		T Dot(const Vector2& other)
		{
			Vector2 temp(*this * other);
			return temp.x + temp.y;
		}

		T Lerp(const Vector2& other, float t)
		{
			return *this + (other - *this) * t;
		}

		Json::Value Serialize() const
		{
			Json::Value root;
			root["x"] = x;
			root["y"] = y;
			return root;
		}

		static bool Deserialize(const Json::Value& root, Vector2& other)
		{
			if (!root.isMember("x") || !root.isMember("y"))
				return false;

			other.x = root["x"].asFloat();
			other.y = root["y"].asFloat();

			return true;
		}
	};

	
	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int32_t>;
	using Vector2u = Vector2<uint32_t>;
}
