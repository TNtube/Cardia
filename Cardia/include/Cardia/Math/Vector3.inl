#pragma once
#include <json/value.h>
#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	template<arithmetic T>
	constexpr Vector3<T>::Vector3(const Vector4<T>& other) noexcept
		: x(other.x), y(other.y), z(other.z) {}
	
	template<arithmetic T>
	constexpr Vector3<T>& Vector3<T>::operator=(const Vector4<T>& other) noexcept
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
	
	template<arithmetic T>
	constexpr T& Vector3<T>::operator[](size_t index) noexcept
	{
		CdCoreAssert(index < this->Size());
		switch(index)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}
	
	template<arithmetic T>
	constexpr T Vector3<T>::operator[](size_t index) const noexcept
	{
		CdCoreAssert(index < this->Size());
		switch(index)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}

	template<arithmetic T>
	constexpr Vector3<T>& Vector3<T>::operator+=(const Vector3& other) noexcept
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector3<T> Vector3<T>::operator+(const Vector3& other) const noexcept
	{
		Vector3 temp(*this);
		temp += other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T>& Vector3<T>::operator+=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x += static_cast<T>(scalar);
		y += static_cast<T>(scalar);
		z += static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T> Vector3<T>::operator+(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector3 temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}

	template <arithmetic T>
	constexpr Vector3<T> Vector3<T>::operator-() const noexcept
	{
		return Vector3(-x, -y, -z);
	}

	template<arithmetic T>
	constexpr Vector3<T>& Vector3<T>::operator-=(const Vector3& other) noexcept
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector3<T> Vector3<T>::operator-(const Vector3& other) const noexcept
	{
		Vector3 temp(*this);
		temp -= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T>& Vector3<T>::operator-=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x -= static_cast<T>(scalar);
		y -= static_cast<T>(scalar);
		z -= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T> Vector3<T>::operator-(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector3 temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr Vector3<T>& Vector3<T>::operator*=(const Vector3& other) noexcept
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector3<T> Vector3<T>::operator*(const Vector3& other) const noexcept
	{
		Vector3 temp(*this);
		temp *= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T>& Vector3<T>::operator*=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x *= static_cast<T>(scalar);
		y *= static_cast<T>(scalar);
		z *= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T> Vector3<T>::operator*(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector3 temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr Vector3<T>& Vector3<T>::operator/=(const Vector3& other) noexcept
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector3<T> Vector3<T>::operator/(const Vector3& other) const noexcept
	{
		Vector3 temp(*this);
		temp /= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T>& Vector3<T>::operator/=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x /= static_cast<T>(scalar);
		y /= static_cast<T>(scalar);
		z /= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector3<T> Vector3<T>::operator/(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector3 temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr T Vector3<T>::Dot(const Vector3& other) noexcept
	{
		Vector3 temp(*this * other);
		return temp.x + temp.y + temp.z;
	}
	
	template<arithmetic T>
	constexpr T Vector3<T>::Length() noexcept
	{
		return sqrt(this->Dot(*this));
	}

	template<arithmetic T>
	constexpr Vector3<T> Vector3<T>::Lerp(const Vector3& other, float t) noexcept
	{
		return *this + (other - *this) * t;
	}

	template<arithmetic T>
	constexpr Vector3<T> Vector3<T>::Cross(const Vector3& other) noexcept
	{
		return Vector3(
			y * other.z - other.y * z,
			z * other.x - other.z * x,
			x * other.y - other.x * y);
	}

	template<arithmetic T>
	constexpr Vector3<T> Vector3<T>::Normalize() const noexcept
	{
		Vector3 temp(*this);

		T len = static_cast<T>(std::sqrt(x * x + y * y + z * z));

		if (len > 0)
		{
			temp.x /= len;
			temp.y /= len;
			temp.z /= len;
		}

		return temp;
	}

	template<arithmetic T>
	Json::Value Vector3<T>::Serialize() const
	{
		Json::Value root;
		root["x"] = x;
		root["y"] = y;
		root["z"] = z;
		return std::move(root);
	}

	template<arithmetic T>
	bool Vector3<T>::Deserialize(const Json::Value& root, Vector3& other)
	{
		if (!root.isMember("x") || !root.isMember("y") || !root.isMember("z"))
			return false;

		other.x = root["x"].asFloat();
		other.y = root["y"].asFloat();
		other.z = root["z"].asFloat();

		return true;
	}
}
