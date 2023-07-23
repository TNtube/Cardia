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
			: x(scalar), y(scalar), z(scalar), w(scalar) {}

		Vector4& operator+=(const Vector4& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		Vector4 operator+(const Vector4& other) const
		{
			Vector4 temp(*this);
			temp += other;
			return temp;
		}

		template<typename U>
		Vector4& operator+=(U scalar) requires std::convertible_to<U, T>
		{
			x += static_cast<T>(scalar);
			y += static_cast<T>(scalar);
			z += static_cast<T>(scalar);
			w += static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector4 operator+(U scalar) const requires std::convertible_to<U, T>
		{
			Vector4 temp(*this);
			temp += static_cast<T>(scalar);
			return temp;
		}

		Vector4 operator-() const
		{
			return Vector4(-x, -y, -z);
		}

		Vector4& operator-=(const Vector4& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		Vector4 operator-(const Vector4& other) const
		{
			Vector4 temp(*this);
			temp -= other;
			return temp;
		}

		template<typename U>
		Vector4& operator-=(U scalar) requires std::convertible_to<U, T>
		{
			x -= static_cast<T>(scalar);
			y -= static_cast<T>(scalar);
			z -= static_cast<T>(scalar);
			w -= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector4 operator-(U scalar) const requires std::convertible_to<U, T>
		{
			Vector4 temp(*this);
			temp -= static_cast<T>(scalar);
			return temp;
		}

		Vector4& operator*=(const Vector4& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;
			return *this;
		}

		Vector4 operator*(const Vector4& other) const
		{
			Vector4 temp(*this);
			temp *= other;
			return temp;
		}

		template<typename U>
		Vector4& operator*=(U scalar) requires std::convertible_to<U, T>
		{
			x *= static_cast<T>(scalar);
			y *= static_cast<T>(scalar);
			z *= static_cast<T>(scalar);
			w *= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector4 operator*(U scalar) const requires std::convertible_to<U, T>
		{
			Vector4 temp(*this);
			temp *= static_cast<T>(scalar);
			return temp;
		}

		Vector4& operator/=(const Vector4& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			w /= other.w;
			return *this;
		}

		Vector4 operator/(const Vector4& other) const
		{
			Vector4 temp(*this);
			temp /= other;
			return temp;
		}

		template<typename U>
		Vector4& operator/=(U scalar) requires std::convertible_to<U, T>
		{
			x /= static_cast<T>(scalar);
			y /= static_cast<T>(scalar);
			z /= static_cast<T>(scalar);
			w /= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		Vector4 operator/(U scalar) const requires std::convertible_to<U, T>
		{
			Vector4 temp(*this);
			temp /= static_cast<T>(scalar);
			return temp;
		}

		T Dot(const Vector4& other)
		{
			Vector4 temp(*this * other);
			return temp.x + temp.y + temp.z + temp.w;
		}

		T Lerp(const Vector4& other, float t)
		{
			return *this + (other - *this) * t;
		}

		Json::Value Serialize() const
		{
			Json::Value root;
			root["x"] = x;
			root["y"] = y;
			root["z"] = z;
			root["w"] = w;
			return root;
		}

		static bool Deserialize(const Json::Value& root, Vector4& other)
		{
			if (!root.isMember("x") || !root.isMember("y") || !root.isMember("z") || !root.isMember("w"))
				return false;

			other.x = root["x"].asFloat();
			other.y = root["y"].asFloat();
			other.z = root["z"].asFloat();
			other.w = root["w"].asFloat();

			return true;
		}
	};

	using Vector4f = Vector4<float>;
	using Vector4i = Vector4<int32_t>;
	using Vector4u = Vector4<uint32_t>;
}
