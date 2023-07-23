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

		Vector3& operator+=(const Vector3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		Vector3 operator+(const Vector3& other) const
		{
			Vector3 temp(*this);
			temp += other;
			return temp;
		}

		template<typename U>
		Vector3& operator+=(U scalar) requires std::convertible_to<U, T>
		{
			x += static_cast<T>(scalar);
			y += static_cast<T>(scalar);
			z += static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector3 operator+(U scalar) const requires std::convertible_to<U, T>
		{
			Vector3 temp(*this);
			temp += static_cast<T>(scalar);
			return temp;
		}

		Vector3 operator-() const
		{
			return Vector3(-x, -y, -z);
		}

		Vector3& operator-=(const Vector3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		Vector3 operator-(const Vector3& other) const
		{
			Vector3 temp(*this);
			temp -= other;
			return temp;
		}

		template<typename U>
		Vector3& operator-=(U scalar) requires std::convertible_to<U, T>
		{
			x -= static_cast<T>(scalar);
			y -= static_cast<T>(scalar);
			z -= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector3 operator-(U scalar) const requires std::convertible_to<U, T>
		{
			Vector3 temp(*this);
			temp -= static_cast<T>(scalar);
			return temp;
		}

		Vector3& operator*=(const Vector3& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		Vector3 operator*(const Vector3& other) const
		{
			Vector3 temp(*this);
			temp *= other;
			return temp;
		}

		template<typename U>
		Vector3& operator*=(U scalar) requires std::convertible_to<U, T>
		{
			x *= static_cast<T>(scalar);
			y *= static_cast<T>(scalar);
			z *= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector3 operator*(U scalar) const requires std::convertible_to<U, T>
		{
			Vector3 temp(*this);
			temp *= static_cast<T>(scalar);
			return temp;
		}

		Vector3& operator/=(const Vector3& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			return *this;
		}

		Vector3 operator/(const Vector3& other) const
		{
			Vector3 temp(*this);
			temp /= other;
			return temp;
		}

		template<typename U>
		Vector3& operator/=(U scalar) requires std::convertible_to<U, T>
		{
			x /= static_cast<T>(scalar);
			y /= static_cast<T>(scalar);
			z /= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector3 operator/(U scalar) const requires std::convertible_to<U, T>
		{
			Vector3 temp(*this);
			temp /= static_cast<T>(scalar);
			return temp;
		}

		T Dot(const Vector3& other)
		{
			Vector3 temp(*this * other);
			return temp.x + temp.y + temp.z;
		}

		Vector3 Cross(const Vector3& other)
		{
			return Vector3(
				y * other.z - other.y * z,
				z * other.x - other.z * x,
				x * other.y - other.x * y);
		}

		T Lerp(const Vector3& other, float t)
		{
			return *this + (other - *this) * t;
		}

		Json::Value Serialize() const
		{
			Json::Value root;
			root["x"] = x;
			root["y"] = y;
			root["z"] = z;
			return root;
		}

		static bool Deserialize(const Json::Value& root, Vector3& other)
		{
			if (!root.isMember("x") || !root.isMember("y") || !root.isMember("z"))
				return false;

			other.x = root["x"].asFloat();
			other.y = root["y"].asFloat();
			other.z = root["z"].asFloat();

			return true;
		}
	};

	using Vector3f = Vector3<float>;
	using Vector3i = Vector3<int32_t>;
	using Vector3u = Vector3<uint32_t>;
}
