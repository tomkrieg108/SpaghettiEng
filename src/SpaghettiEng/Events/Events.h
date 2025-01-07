#pragma once
#include <Common/Common.h>
//#include "Base.h"

namespace Spg
{
  enum class EventType : uint32_t
  {
    WindowClose, WindowResize, WindowMove, WindowFocusChange, WindowHoverChange, WindowIconifyChange,
    ViewportResize,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
    KeyPressed, KeyReleased, KeyTyped,
		Count,
    None,
  };

  struct Event
  {
		virtual ~Event() = default;
		virtual EventType GetType() const { return EventType::None; }
		bool handled = false;
  };

  //WINDOW EVENTS

  struct EventWindowClose : public Event
  {
      EventWindowClose() = default;
      EventType GetType() const override { return EventType::WindowClose; }
      static EventType GetStaticType() { return EventType::WindowClose; }
  };

  struct EventWindowResize : public Event
  {
      EventWindowResize() = delete;
      EventWindowResize(int buffer_width, int buffer_height) : buffer_width{ buffer_width }, buffer_height{ buffer_height } {}
      EventType GetType() const override { return EventType::WindowResize; }
      static EventType GetStaticType() { return EventType::WindowResize; }
      int buffer_width = 0, buffer_height = 0;
  };

  struct EventWindowFocusChange : public Event
  {
      EventWindowFocusChange() = delete;
      EventWindowFocusChange(int focused) : has_focus { focused } {}
      EventType GetType() const override { return EventType::WindowFocusChange; }
      static EventType GetStaticType() { return EventType::WindowFocusChange; }
      int has_focus = 0;
  };

  struct EventWindowHoverChange : public Event
  {
      EventWindowHoverChange() = delete;
      EventWindowHoverChange(int hovered) : hovered{ hovered } {}
      EventType GetType() const override { return EventType::WindowHoverChange; }
      static EventType GetStaticType() { return EventType::WindowHoverChange; }
      int hovered = 0;
  };

  struct EventWindowIconifyChange : public Event
  {
      EventWindowIconifyChange() = delete;
      EventWindowIconifyChange(int iconified) : is_iconified{ iconified } {}
      EventType GetType() const override { return EventType::WindowIconifyChange; }
      static EventType GetStaticType() { return EventType::WindowIconifyChange; }
      int is_iconified = 0;
  };

  struct EventWindowMove : public Event
  {
      EventWindowMove() = delete;
      EventWindowMove(int xpos, int ypos) : xpos{ xpos }, ypos{ ypos } {}
      EventType GetType() const override { return EventType::WindowMove; }
      static EventType GetStaticType() { return EventType::WindowMove; }
      int xpos = 0, ypos = 0;
  };

  struct EventViewportResize : public Event
  {
      EventViewportResize() = delete;
      EventViewportResize(int width, int height) : width{ width }, height{ height } {}
      EventType GetType() const override { return EventType::ViewportResize; }
      static EventType GetStaticType() { return EventType::ViewportResize; }
      int width = 0, height = 0;
  };

  //MOUSE EVENTS

  struct EventMouseButtonPressed : public Event
  {
		EventMouseButtonPressed() = delete;
		EventMouseButtonPressed(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {}
		EventType GetType() const override { return EventType::MouseButtonPressed; }
    static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		float x = 0, y = 0;
		int btn = 0;
  };

  struct EventMouseButtonReleased : public Event
  {
		EventMouseButtonReleased() = delete;
		EventMouseButtonReleased(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {}
		EventType GetType() const override { return EventType::MouseButtonReleased; }
    static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		float x = 0, y = 0;
		int btn = 0;
  };

  struct EventMouseMoved : public Event
  {
    EventMouseMoved() = delete;
    EventMouseMoved(float x, float y, float delta_x, float delta_y) :
      x{ x }, y{y}, delta_x {delta_x}, delta_y {delta_y} {}
    EventType GetType() const override { return EventType::MouseMoved; }
    static EventType GetStaticType() { return EventType::MouseMoved; }
    float x, y;
    float delta_x, delta_y;
  };

  struct EventMouseScrolled : public Event
  {
    EventMouseScrolled() = delete;
    EventMouseScrolled(float x_offset, float y_offset) :
      x_offset{ x_offset }, y_offset{ y_offset } {}
    EventType GetType() const override { return EventType::MouseScrolled; }
    static EventType GetStaticType() { return EventType::MouseScrolled; }
    float x_offset, y_offset;
  };

  // KEY EVENTS

  struct EventKeyPressed : public Event
  {
      EventKeyPressed() = delete;
      EventKeyPressed(int key, bool repeated = false) : key{ key }, repeated{ repeated } {}
      EventType GetType() const override { return EventType::KeyPressed; }
      static EventType GetStaticType() { return EventType::KeyPressed; }
      int key = 0;
      bool repeated = false;
  };

  struct EventKeyReleased : public Event
  {
      EventKeyReleased() = delete;
      EventKeyReleased(int key) : key{ key } {}
      EventType GetType() const override { return EventType::KeyReleased; }
      static EventType GetStaticType() { return EventType::KeyReleased; }
      int key = 0;
  };

  struct EventKeyTyped : public Event
  {
      EventKeyTyped() = delete;
      EventKeyTyped(int key) : key{ key } {}
      EventType GetType() const override { return EventType::KeyTyped; }
      static EventType GetStaticType() { return EventType::KeyTyped; }
      int key;
  };

}