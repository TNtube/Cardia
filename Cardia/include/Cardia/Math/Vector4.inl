#pragma once
#include <json/value.h>
#include "Cardia/Core/Concepts.hpp"
#include "Cardia/Core/Core.hpp"

namespace Cardia
{

	template<arithmetic T>
	constexpr T Vector4<T>::operator[](size_t index) const noexcept
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
			case 3:
				return w;
		}
	}
	
	template<arithmetic T>
	constexpr T& Vector4<T>::operator[](size_t index) noexcept
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
		case 3:
			return w;
		}
	}

	template<arithmetic T>
	constexpr Vector4<T>& Vector4<T>::operator+=(const Vector4& other) noexcept
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector4<T> Vector4<T>::operator+(const Vector4& other) const noexcept
	{
		Vector4 temp(*this);
		temp += other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T>& Vector4<T>::operator+=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x += static_cast<T>(scalar);
		y += static_cast<T>(scalar);
		z += static_cast<T>(scalar);
		w += static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T> Vector4<T>::operator+(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}

	template <arithmetic T>
	constexpr Vector4<T> Vector4<T>::operator-() const noexcept
	{
		return Vector4(-x, -y, -z, -w);
	}

	template<arithmetic T>
	constexpr Vector4<T>& Vector4<T>::operator-=(const Vector4& other) noexcept
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector4<T> Vector4<T>::operator-(const Vector4& other) const noexcept
	{
		Vector4 temp(*this);
		temp -= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T>& Vector4<T>::operator-=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x -= static_cast<T>(scalar);
		y -= static_cast<T>(scalar);
		z -= static_cast<T>(scalar);
		w -= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T> Vector4<T>::operator-(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr Vector4<T>& Vector4<T>::operator*=(const Vector4& other) noexcept
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector4<T> Vector4<T>::operator*(const Vector4& other) const noexcept
	{
		Vector4 temp(*this);
		temp *= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T>& Vector4<T>::operator*=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x *= static_cast<T>(scalar);
		y *= static_cast<T>(scalar);
		z *= static_cast<T>(scalar);
		w *= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T> Vector4<T>::operator*(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr Vector4<T>& Vector4<T>::operator/=(const Vector4& other) noexcept
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector4<T> Vector4<T>::operator/(const Vector4& other) const noexcept
	{
		Vector4 temp(*this);
		temp /= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T>& Vector4<T>::operator/=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x /= static_cast<T>(scalar);
		y /= static_cast<T>(scalar);
		z /= static_cast<T>(scalar);
		w /= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector4<T> Vector4<T>::operator/(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr T Vector4<T>::Dot(const Vector4& other) noexcept
	{
		Vector4 temp(*this * other);
		return temp.x + temp.y + temp.z + temp.w;
	}

	template<arithmetic T>
	constexpr T Vector4<T>::Length() noexcept
	{
		return sqrt(this->Dot(*this));
	}

	template<arithmetic T>
	constexpr Vector4<T> Vector4<T>::Lerp(const Vector4& other, float t) noexcept
	{
		return *this + (other - *this) * t;
	}

	template<arithmetic T>
	constexpr Vector4<T> Vector4<T>::Normalize() noexcept
	{
		Vector4 temp(*this);

		T len = static_cast<T>(std::sqrt(x * x + y * y + z * z + w * w));

		if constexpr (len > 0)
		{
			temp.x /= len;
			temp.y /= len;
			temp.z /= len;
			temp.w /= len;
		}

		return temp;
	}

	template<arithmetic T>
	Json::Value Vector4<T>::Serialize() const
	{
		Json::Value root;
		root["x"] = x;
		root["y"] = y;
		root["z"] = z;
		root["w"] = w;
		return std::move(root);
	}

	template<arithmetic T>
	bool Vector4<T>::Deserialize(const Json::Value& root, Vector4& other)
	{
		if (!root.isMember("x") || !root.isMember("y") || !root.isMember("z") || !root.isMember("w"))
			return false;

		other.x = root["x"].asFloat();
		other.y = root["y"].asFloat();
		other.z = root["z"].asFloat();
		other.w = root["w"].asFloat();

		return true;
	}
}
