#pragma once
#include "Base.h"

namespace Spg
{
  enum class EventType : uint32_t
  {
    None,
    WindowClose, WindowResize, WindowMove, WindowFocusChange, WindowHoverChange, WindowIconifyChange,
    ViewportResize,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
  };

  enum EventCategory
  {
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeyboard = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4)
  };

  struct Event
  {
		virtual ~Event() = default;
		virtual EventType Type() const { return EventType::None; }
		virtual int GetCategoryFlags() const = 0;
		virtual const char* GetName() const { return ""; }
		virtual std::string ToString() const { return GetName(); }
		bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
		
		bool handled = false;
  };

  struct EventMouseButtonPressed : public Event
  {
		EventMouseButtonPressed() = delete;
		EventMouseButtonPressed(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {}

		EventType Type() const override { return EventType::MouseButtonPressed; }
    static EventType StaticType() { return EventType::MouseButtonPressed; }
		int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput; }
		const char* GetName() const override { return "Mouse Button Pressed: "; }
		std::string ToString() const override
		{
			std::string s = std::string{ GetName() };
			s += "Btn: " + std::to_string(btn) + " Pos: " + "( " + std::to_string(x) + "," + std::to_string(y) + ")";
			return s;
		}

		float x = 0, y = 0;
		int btn = 0;
  };

  struct EventMouseButtonReleased : public Event
  {
		EventMouseButtonReleased() = delete;
		EventMouseButtonReleased(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {}

		EventType Type() const override { return EventType::MouseButtonReleased; }
    static EventType StaticType() { return EventType::MouseButtonReleased; }
		int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput; }
		const char* GetName() const override { return "Mouse Button Releasd: "; }
		std::string ToString() const override
		{
			std::string s = std::string{ GetName() };
			s += "Btn: " + std::to_string(btn) + " Pos: " + "( " + std::to_string(x) + "," + std::to_string(y) + ")";
			return s;
		}

		float x = 0, y = 0;
		int btn = 0;
  };

  


}