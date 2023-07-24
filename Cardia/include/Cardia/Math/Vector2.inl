#pragma once
#include <json/value.h>
#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	template<arithmetic T>
	T& Vector2<T>::operator[](size_t index)
	{
		CdCoreAssert(index < this->length());
		switch(index)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		}
	}

	template<arithmetic T>
	Vector2<T>& Vector2<T>::operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	template<arithmetic T>
	Vector2<T> Vector2<T>::operator+(const Vector2& other) const
	{
		Vector2 temp(*this);
		temp += other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T>& Vector2<T>::operator+=(U scalar) requires std::convertible_to<U, T>
	{
		x += static_cast<T>(scalar);
		y += static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T> Vector2<T>::operator+(U scalar) const requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}

	template <arithmetic T>
	Vector2<T> Vector2<T>::operator-() const
	{
		return Vector2(-x, -y);
	}

	template<arithmetic T>
	Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	template<arithmetic T>
	Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const
	{
		Vector2 temp(*this);
		temp -= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T>& Vector2<T>::operator-=(U scalar) requires std::convertible_to<U, T>
	{
		x -= static_cast<T>(scalar);
		y -= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T> Vector2<T>::operator-(U scalar) const requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	template<arithmetic T>
	Vector2<T> Vector2<T>::operator*(const Vector2<T>& other) const
	{
		Vector2 temp(*this);
		temp *= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T>& Vector2<T>::operator*=(U scalar) requires std::convertible_to<U, T>
	{
		x *= static_cast<T>(scalar);
		y *= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T> Vector2<T>::operator*(U scalar) const requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	Vector2<T>& Vector2<T>::operator/=(const Vector2<T>& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	template<arithmetic T>
	Vector2<T> Vector2<T>::operator/(const Vector2<T>& other) const
	{
		Vector2 temp(*this);
		temp /= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T>& Vector2<T>::operator/=(U scalar) requires std::convertible_to<U, T>
	{
		x /= static_cast<T>(scalar);
		y /= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	Vector2<T> Vector2<T>::operator/(U scalar) const requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	T Vector2<T>::Dot(const Vector2& other)
	{
		Vector2 temp(*this * other);
		return temp.x + temp.y;
	}

	template<arithmetic T>
	Vector2<T> Vector2<T>::Lerp(const Vector2& other, float t)
	{
		return *this + (other - *this) * t;
	}

	template<arithmetic T>
	Json::Value Vector2<T>::Serialize() const
	{
		Json::Value root;
		root["x"] = x;
		root["y"] = y;
		return std::move(root);
	}

	template<arithmetic T>
	bool Vector2<T>::Deserialize(const Json::Value& root, Vector2& other)
	{
		if (!root.isMember("x") || !root.isMember("y"))
			return false;

		other.x = root["x"].asFloat();
		other.y = root["y"].asFloat();

		return true;
	}
}
