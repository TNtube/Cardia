#pragma once

#include "Cardia/Core/Concepts.hpp"
#include "Vector3.hpp"


namespace Cardia
{
	template <floating_point T>
	constexpr Quaternion<T>::Quaternion(Vector3<T> euler) noexcept
	{
		auto halfEuler = euler * static_cast<T>(0.5);

		Vector3<T> c { cos(halfEuler.x), cos(halfEuler.y), cos(halfEuler.z) };
		Vector3<T> s { sin(halfEuler.x), sin(halfEuler.y), sin(halfEuler.z) };

		this->m_Real = c.x * c.y * c.z + s.x * s.y * s.z;
		this->m_Imaginary = {
			s.x * c.y * c.z - c.x * s.y * s.z,
			c.x * s.y * c.z + s.x * c.y * s.z,
			c.x * c.y * s.z - s.x * s.y * c.z
		};
	}

	// http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
	template <floating_point T>
	constexpr Quaternion<T>::Quaternion(Vector3<T> u, Vector3<T> v) noexcept
	{
		T norm_u_norm_v = sqrt(u.Dot(u) * v.Dot(v));
		T real_part = norm_u_norm_v + u.Dot(v);
		Vector3<T> t;

		if(real_part < static_cast<T>(1.e-6f) * norm_u_norm_v)
		{
			// If u and v are exactly opposite, rotate 180 degrees
			// around an arbitrary orthogonal axis. Axis normalisation
			// can happen later, when we normalise the quaternion.
			real_part = static_cast<T>(0);
			t = abs(u.x) > abs(u.z) ? Vector3<T>(-u.y, u.x, static_cast<T>(0))
						: Vector3<T>(static_cast<T>(0), -u.z, u.y);
		}
		else
		{
			// Otherwise, build quaternion the standard way.
			t = u.Cross(v);
		}

		*this = Quaternion(real_part, t).Normalize();
	}

	template <floating_point T>
	constexpr T Quaternion<T>::Length() const noexcept
	{
		return sqrt(this->Dot(*this));
	}

	template <floating_point T>
	constexpr T Quaternion<T>::Dot(const Quaternion& other) const noexcept
	{
		return m_Real * other.m_Real + m_Imaginary.Dot(other.m_Imaginary);
	}

	template <floating_point T>
	constexpr Quaternion<T> Quaternion<T>::Normalize() noexcept
	{
		T len = Length();
		if constexpr (len <= static_cast<T>(0)) // Problem
			return Quaternion(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		T oneOverLen = static_cast<T>(1) / len;

		return Quaternion(m_Real * oneOverLen, m_Imaginary * oneOverLen);
	}

	template <floating_point T>
	constexpr Matrix4<T> Quaternion<T>::ToMatrix() const noexcept
	{
		Matrix4<T> result(static_cast<T>(1));
		T xx(m_Imaginary.x * m_Imaginary.x);
		T yy(m_Imaginary.y * m_Imaginary.y);
		T zz(m_Imaginary.z * m_Imaginary.z);
		T xz(m_Imaginary.x * m_Imaginary.z);
		T xy(m_Imaginary.x * m_Imaginary.y);
		T yz(m_Imaginary.y * m_Imaginary.z);
		T wx(m_Real * m_Imaginary.x);
		T wy(m_Real * m_Imaginary.y);
		T wz(m_Real * m_Imaginary.z);

		result[0][0] = static_cast<T>(1) - static_cast<T>(2) * (yy +  zz);
		result[0][1] = static_cast<T>(2) * (xy + wz);
		result[0][2] = static_cast<T>(2) * (xz - wy);

		result[1][0] = static_cast<T>(2) * (xy - wz);
		result[1][1] = static_cast<T>(1) - static_cast<T>(2) * (xx +  zz);
		result[1][2] = static_cast<T>(2) * (yz + wx);

		result[2][0] = static_cast<T>(2) * (xz + wy);
		result[2][1] = static_cast<T>(2) * (yz - wx);
		result[2][2] = static_cast<T>(1) - static_cast<T>(2) * (xx +  yy);

		return result;
	}

}
