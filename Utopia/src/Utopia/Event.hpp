#pragma once


#include "Utopia/Core.hpp"

namespace Utopia
{
	/* Events are, at the moment, blocking. When an event is happening, he is
	 * immediately dispatched and must be treated right now.
	 * In a future, a better idea should be to buffer events in ane event queue
	 * and treat them during the event part of the update system.
	 */

	
	enum class EventType
	{
		Null = 0,
		WinClose, WinResize, WinFocus, WinLostFocus, WinMoved,
		KeyDown, KeyUp,
		AppTick, AppUpdate, AppRender,
		MouseButtonDown, MouseButtonUp, MouseMotion, MouseScrolled
	};

	enum class EventCategory
	{
		Null = 0,
		EventCatApplication		= bit(0),
		EventCatInput			= bit(1),
		EventCatKeyboard		= bit(2),
		EventCatMouse			= bit(3),
		EventCatMouseButton		= bit(4)
	};

	EventCategory operator|(EventCategory a, EventCategory b)
	{
		return static_cast<EventCategory>(enum_as_integer(a) | enum_as_integer(b));
	}

	// Creating bunch of define to easily implement subclasses of the Event class
#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::##type; }\
								virtual EventType getEventType() const override { return getStaticType(); }\
								virtual const char* getName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return enum_as_integer(category); }
	

	class UTOPIA_API Event
	{
		 // The Event class is a sort of abstract class, that is here to represent any type. 
		
		friend class EventDispatcher;
	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0; 
		
		virtual std::string toString() const { return getName(); }		// for debugging purpose

		inline bool isInCategory(Utopia::EventCategory category)
		{
			return getCategoryFlags() & enum_as_integer(category);
		}
	protected:
		bool m_Handled = false;
	};
	
	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		template<typename T>
		bool dispatch(std::function<bool(T&)> func)
		{
			if (m_Event.getEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		// Used to auto format Event to be used by spdlog
		return os << e.toString();
	}


	// -------------------------------------- Application Events --------------------------------------

	class UTOPIA_API WinResizeEvent : public Event
	{
	public:
		WinResizeEvent(unsigned width, unsigned height)
			: m_Width(width), m_Height(height) {}

		std::string toString() const override							// for debugging purpose
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WinResize)
		EVENT_CLASS_CATEGORY(EventCategory::EventCatApplication)

	private:
		unsigned m_Width, m_Height;
	};

	class UTOPIA_API WinCloseEvent : public Event
	{
	public:
		WinCloseEvent() = default;

		EVENT_CLASS_TYPE(WinClose)
		EVENT_CLASS_CATEGORY(EventCategory::EventCatApplication)
	};

	class UTOPIA_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategory::EventCatApplication)
	};

	class UTOPIA_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategory::EventCatApplication)
	};

	class UTOPIA_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategory::EventCatApplication)
	};


	// -------------------------------------- Keys Events --------------------------------------

	class UTOPIA_API KeyEvent : public Event
	{
		// KeyEvent is an abstract class to encompass Keys Events subclasses
	public:
		inline int getKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategory::EventCatKeyboard | EventCategory::EventCatInput)

	protected:
		KeyEvent(int keyCode)
			: m_KeyCode(keyCode) {}

		int m_KeyCode;
	};


	class UTOPIA_API KeyDownEvent : public KeyEvent
	{
	public:
		KeyDownEvent(int keyCode, int repeatCount)
			: KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

		inline int getRepeatCount() const { return m_RepeatCount; }

		std::string toString() const override						// for debugging purpose
		{
			std::stringstream ss;
			ss << "KeyDownEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();

		}

		EVENT_CLASS_TYPE(KeyDown)
	private:
		int m_RepeatCount;
	};

	class UTOPIA_API KeyUpEvent : public KeyEvent
	{
	public:
		KeyUpEvent(int keyCode)
			: KeyEvent(keyCode) {}

		std::string toString() const override						// for debugging purpose
		{
			std::stringstream ss;
			ss << "KeyUpEvent: " << m_KeyCode;
			return ss.str();

		}

		EVENT_CLASS_TYPE(KeyDown)
	};

	// -------------------------------------- Mouse Events --------------------------------------

	class UTOPIA_API MouseMotionEvent : public Event
	{
	public:
		MouseMotionEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float getX() const { return m_MouseX; }
		inline float getY() const { return m_MouseY; }

		std::string toString() const override						// for debugging purpose
		{
			std::stringstream ss;
			ss << "MouseMotionEvent: (" << m_MouseX << ", " << m_MouseY << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMotion)
		EVENT_CLASS_CATEGORY(EventCategory::EventCatMouse | EventCategory::EventCatInput)

	private:
		float m_MouseX, m_MouseY;
	};

	class UTOPIA_API MouseScolledEvent : public Event
	{
	public:
		MouseScolledEvent(float x, float y)
			: m_OffSetX(x), m_OffSetY(y) {}

		std::string toString() const override						// for debugging purpose
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: (" << m_OffSetX << ", " << m_OffSetY << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategory::EventCatMouse | EventCategory::EventCatInput)
	private:
		float m_OffSetX, m_OffSetY;
	};

	class UTOPIA_API MouseButtonEvent : public Event
	{
		// KeyEvent is an abstract class to encompass Keys Events subclasses
	public:
		inline int getButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategory::EventCatMouse | EventCategory::EventCatInput)

	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}

		int m_Button;
	};


	class UTOPIA_API MouseButtonDownEvent : public MouseButtonEvent
	{
	public:
		MouseButtonDownEvent(int button)
			: MouseButtonEvent(button) {}

		std::string toString() const override						// for debugging purpose
		{
			std::stringstream ss;
			ss << "MouseButtonDownEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonDown)
	};

	class UTOPIA_API MouseButtonUpEvent : public MouseButtonEvent
	{
	public:
		MouseButtonUpEvent(int button)
			: MouseButtonEvent(button) {}

		std::string toString() const override						// for debugging purpose
		{
			std::stringstream ss;
			ss << "MouseButtonUpEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonUp)
	};
}
