#pragma once


namespace Cardia {
	class DeltaTime {
	public:
		DeltaTime(float time = 0.0f)
			: m_Time(time) {}

		inline float AsSeconds() const { return m_Time; }

		inline float AsMilliseconds() const { return m_Time * 1000.0f; }

		operator float() const { return m_Time; }

	private:
		float m_Time;
	};

	class Application;

	class Time {
	public:
		static DeltaTime GetDeltaTime() { return m_DeltaTime; }
		static double GetTime() { return m_LastFrameTime; }
	private:
		friend Application;
		static DeltaTime m_DeltaTime;
		static double m_LastFrameTime;
	};
}