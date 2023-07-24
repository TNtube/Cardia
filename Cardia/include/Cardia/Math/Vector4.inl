#pragma once
#include <json/value.h>
#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	template<arithmetic T>
	Vector4<T>& Vector4<T>::operator+=(const Vector4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	template<arithmetic T>
	Vector4<T> Vector4<T>::operator+(const Vector4& other) const
	{
		Vector4 temp(*this);
		temp += other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T>& Vector4<T>::operator+=(U scalar) requires std::convertible_to<U, T>
	{
		x += static_cast<T>(scalar);
		y += static_cast<T>(scalar);
		z += static_cast<T>(scalar);
		w += static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T> Vector4<T>::operator+(U scalar) const requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}

	template <arithmetic T>
	Vector4<T> Vector4<T>::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	template<arithmetic T>
	Vector4<T>& Vector4<T>::operator-=(const Vector4& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	template<arithmetic T>
	Vector4<T> Vector4<T>::operator-(const Vector4& other) const
	{
		Vector4 temp(*this);
		temp -= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T>& Vector4<T>::operator-=(U scalar) requires std::convertible_to<U, T>
	{
		x -= static_cast<T>(scalar);
		y -= static_cast<T>(scalar);
		z -= static_cast<T>(scalar);
		w -= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T> Vector4<T>::operator-(U scalar) const requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	Vector4<T>& Vector4<T>::operator*=(const Vector4& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}

	template<arithmetic T>
	Vector4<T> Vector4<T>::operator*(const Vector4& other) const
	{
		Vector4 temp(*this);
		temp *= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T>& Vector4<T>::operator*=(U scalar) requires std::convertible_to<U, T>
	{
		x *= static_cast<T>(scalar);
		y *= static_cast<T>(scalar);
		z *= static_cast<T>(scalar);
		w *= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T> Vector4<T>::operator*(U scalar) const requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	Vector4<T>& Vector4<T>::operator/=(const Vector4& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

	template<arithmetic T>
	Vector4<T> Vector4<T>::operator/(const Vector4& other) const
	{
		Vector4 temp(*this);
		temp /= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T>& Vector4<T>::operator/=(U scalar) requires std::convertible_to<U, T>
	{
		x /= static_cast<T>(scalar);
		y /= static_cast<T>(scalar);
		z /= static_cast<T>(scalar);
		w /= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector4<T> Vector4<T>::operator/(U scalar) const requires std::convertible_to<U, T>
	{
		Vector4 temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	T Vector4<T>::Dot(const Vector4& other)
	{
		Vector4 temp(*this * other);
		return temp.x + temp.y + temp.z + temp.w;
	}

	template<arithmetic T>
	Vector4<T> Vector4<T>::Lerp(const Vector4& other, float t)
	{
		return *this + (other - *this) * t;
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
