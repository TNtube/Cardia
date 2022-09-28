#pragma once


namespace Cardia {
	class DeltaTime {
	public:
		DeltaTime(float time = 0.0f)
			: m_Time(time) {}

		inline float seconds() const { return m_Time; }

		inline float milliseconds() const { return m_Time * 1000.0f; }

		operator float() const { return m_Time; }

	private:
		float m_Time;
	};

	class Application;

	class Time {
	public:
		static DeltaTime deltaTime() { return m_DeltaTime; };
	private:
		friend Application;
		static DeltaTime m_DeltaTime;
	};
}