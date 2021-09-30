#pragma once
#include <memory>


namespace Cardia
{
	class DeltaTime
	{
	public:
		DeltaTime(float time = 0.0f)
			: m_Time(time) {}

		inline float seconds() const { return m_Time; }
		inline float milliseconds() const { return m_Time * 1000.0f; }

	private:
		float m_Time;
	};
}