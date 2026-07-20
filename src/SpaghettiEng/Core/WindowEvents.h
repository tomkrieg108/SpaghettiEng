#pragma once

//#include <variant>
#include <type_traits>
#include <functional>

// {}

namespace Spg
{
  namespace WinEvt
  {

    enum class EventType
    {
      WindowClose, WindowResize, WindowMove, WindowFocusChange, WindowHoverChange, WindowIconifyChange,
      ViewportResize,
      MouseBtnPressed, MouseBtnReleased, MouseMoved, MouseScrolled,
      KeyPressed, KeyReleased, KeyTyped,
      Count,
      None,
    };

    struct Event
    {
      EventType type = EventType::None;
      bool handled = false;
    };


    struct WindowClose : public Event
    {
        WindowClose() {type = GetStaticType(); }
        static EventType GetStaticType() { return EventType::WindowClose; }
    };

    struct WindowResize : public Event
    {
        WindowResize() = delete;
        WindowResize(int buffer_width, int buffer_height) : buffer_width{ buffer_width }, buffer_height{ buffer_height } {type = GetStaticType();}
        static EventType GetStaticType() { return EventType::WindowResize; }
        int buffer_width = 0, buffer_height = 0;
    };

    struct WindowFocusChange : public Event
    {
        WindowFocusChange() = delete;
        WindowFocusChange(int focused) : has_focus { focused } {type = GetStaticType();}
        static EventType GetStaticType() { return EventType::WindowFocusChange; }
        int has_focus = 0;
    };

    struct WindowHoverChange : public Event
    {
        WindowHoverChange() = delete;
        WindowHoverChange(int hovered) : hovered{ hovered } {type = GetStaticType();}
        static EventType GetStaticType() { return EventType::WindowHoverChange; }
        int hovered = 0;
    };

    struct WindowIconifyChange : public Event
    {
        WindowIconifyChange() = delete;
        WindowIconifyChange(int iconified) : is_iconified{ iconified } {type = GetStaticType();}
        static EventType GetStaticType() { return EventType::WindowIconifyChange; }
        int is_iconified = 0;
    };

    struct WindowMove : public Event
    {
        WindowMove() = delete;
        WindowMove(int xpos, int ypos) : xpos{ xpos }, ypos{ ypos } {type = GetStaticType();}
        static EventType GetStaticType() { return EventType::WindowMove; }
        int xpos = 0, ypos = 0;
    };

    struct WindowViewportResize : public Event
    {
      WindowViewportResize() = delete;
      WindowViewportResize(int width, int height) : width{ width }, height{ height } {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::ViewportResize; }
      int width = 0, height = 0;
    };

    //MOUSE EVENTS

    struct MouseBtnPressed : public Event
    {
      MouseBtnPressed() = delete;
      MouseBtnPressed(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::MouseBtnPressed; }
      float x = 0, y = 0;
      int btn = 0;
    };

    struct MouseBtnReleased : public Event
    {
      MouseBtnReleased() = delete;
      MouseBtnReleased(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::MouseBtnReleased; }
      float x = 0, y = 0;
      int btn = 0;
    };

    struct MouseMoved : public Event
    {
      MouseMoved() = delete;
      MouseMoved(float x, float y, float delta_x, float delta_y) :
        x{ x }, y{y}, delta_x {delta_x}, delta_y {delta_y} {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::MouseMoved; }
      float x, y;
      float delta_x, delta_y;
    };

    struct MouseScrolled : public Event
    {
      MouseScrolled() = delete;
      MouseScrolled(float x_offset, float y_offset) :
        x_offset{ x_offset }, y_offset{ y_offset } {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::MouseScrolled; }
      float x_offset, y_offset;
    };

    // KEY EVENTS

    struct KeyPressed : public Event
    {
      KeyPressed() = delete;
      KeyPressed(int key, bool repeated = false) : key{ key }, repeated{ repeated } {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::KeyPressed; }
      int key = 0;
      bool repeated = false;
    };

    struct KeyReleased : public Event
    {
      KeyReleased() = delete;
      KeyReleased(int key) : key{ key } {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::KeyReleased; }
      int key = 0;
    };

    struct KeyTyped : public Event
    {
      KeyTyped() = delete;
      KeyTyped(int key) : key{ key } {type = GetStaticType();}
      static EventType GetStaticType() { return EventType::KeyTyped; }
      int key;
    };


    class Dispatcher
    {
      public:
        Dispatcher(Event& event) : m_event(event) {}

        template<typename E, typename F>
        //requires std::invocable<F, int>
        bool Dsipatch(const F& func)
        {
          static_assert(std::is_base_of_v<Event,E>);
          //static_assert(std::is_invocable_r_v<void, decltype(func),static_cast<E&>(m_event)>);

          if(m_event.type == E::GetStaticType())
          {
            // m_event.handled |= func(static_cast<E&>(m_event));
            // return true;
            func(static_cast<E&>(m_event));
            return true;
          }
          return false;  
        }

      private:
        Event& m_event;
    };

    
    using Callback = std::function<void(Event&)>;
    
    template<typename T>
    using MemberFn = void(T::*)(Event&); 

    
    template<typename T>
    static Callback MakeCallback(T* instance, MemberFn<T> func)
    {
      Callback callback = [instance, func](Event& event){ 
        return (instance->*func)(event);
      };
      return callback;
    }

   
  // Alternatively
  #if 0
    template<typename T, typename E>
    class WinEventCallback2 {
    
    public:
        // Constructor stores the specific pointers
        WinEventCallback2(T* inst, void(T::*f)(E&)) : instance(inst), func(f) {}

        // Overloading operator() makes this class a functor
        void operator()(E& event) const {
            (instance->*func)(event); // Direct, standard member pointer call
        }

     private:
        T* instance;
        void(T::*func)(E&);   
    };

    
    template<typename T>
    static auto MakeMemberCallback2(T* instance, MemberFuncType<T,WindowEvent> func) {
        return MakeMemberCallback2<T, WindowEvent>(instance, func);
    }
  #endif 

  } //namespace WindowEvent
  

} //namespace Spg
  
