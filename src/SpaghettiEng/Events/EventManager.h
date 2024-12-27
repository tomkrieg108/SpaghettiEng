#pragma once

#include <type_traits>
#include <variant>
#include <array>
#include <queue>
#include <iostream>

#include "Core/Base.h"
#include "Events.h"


namespace Spg
{
 
 inline namespace Evt_2
  {

    //This is what Hazel eng does for to define a handler
    #define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

    template<typename T, typename E>
    using MemberFuncType = void(T::*)(E&);

    template<typename E>
    using FreeFuncType =  void(*)(E&);

    class EventHandler
    {
    public:
      virtual ~EventHandler() = default;
      virtual void Dispatch(Event& event) = 0;
    };

    template<typename E>
    class EventHandlerFreeFunc : public EventHandler
    {
    public:
      EventHandlerFreeFunc(FreeFuncType<E> handler) : m_handler{handler} {}
      void Dispatch(Event& event) override { m_handler( static_cast<E&>(event) );}

    private:
      FreeFuncType<E> m_handler;
    };

    template<typename T, typename E>
    class EventHandlerMemberFunc : public EventHandler
    {
    public:
      EventHandlerMemberFunc(T* instance, MemberFuncType<T,E> handler) : m_instance{instance},m_handler{handler} {}
      void Dispatch(Event& event) override { (m_instance->*m_handler)( static_cast<E&>(event) );}
    private:
      T* m_instance;
      MemberFuncType<T,E> m_handler;
    };
   
  //------------------------------------------
    class EventManager
    {

    private:
      struct EventWrapper
      {
        EventType type;    

       
        // using EventVariant = 
        //  std::variant<EventWindowClose, EventWindowResize, EventWindowMove, EventWindowFocusChange, EventWindowHoverChange, EventWindowIconifyChange, EventViewportResize, EventMouseButtonPressed, EventMouseButtonReleased, EventMouseMoved, EventMouseScrolled,EventKeyPressed, EventKeyReleased, EventKeyTyped>;

        //These are the only ones we care about for now
        using EventVariant = std::variant<EventWindowClose, EventWindowResize, EventMouseButtonPressed, EventMouseButtonReleased, EventMouseMoved, EventMouseScrolled, EventKeyPressed, EventKeyReleased>;

        EventVariant event;
      };

    public:

      //todo - constructors should be private or deleted
      EventManager();

      template<typename T, typename E>
      static void AddHandler(T* instance, MemberFuncType<T,E> handler_func)
      {
        EventHandler* handler = new EventHandlerMemberFunc<T,E>(instance, handler_func);
        s_instance.m_handler_list[(uint32_t)E::GetStaticType()] = handler;
      }

      template<typename E>
      static void AddHandler(FreeFuncType<E> handler_func)
      {
        EventHandler* handler = new EventHandlerFreeFunc<E>(handler_func);
        s_instance.m_handler_list[(uint32_t)E::GetStaticType()] = handler;    
      }

      static void Initialise();
      static void DispatchQueuedEvents();

      template<typename E>
      static void Dispatch(E& event)
      {
        static_assert(std::derived_from<E, Event>);
        uint32_t type = (uint32_t)(E::GetStaticType());
        auto handler = s_instance.m_handler_list[type];  
        SPG_ASSERT(handler != nullptr);
        handler->Dispatch(event);
      }

      template<typename E>
      static void Enqueue(E& event)
      {
        static_assert(std::derived_from<E, Event>);
        s_instance.m_event_queue.push({E::GetStaticType(), event});

        #ifdef SPG_DEBUG
        if(s_instance.m_event_queue.size() > s_instance.m_max_queue_size)
          s_instance.m_max_queue_size = s_instance.m_event_queue.size();
        #endif
      }

      #ifdef SPG_DEBUG
        static uint32_t GetMaxQueueSize() { return s_instance.m_max_queue_size; }
        static uint32_t GetCurrentQueueSize() {return (uint32_t)s_instance.m_event_queue.size();}
      #endif

    private: 
      std::array<EventHandler*, (size_t)EventType::Count> m_handler_list;
      std::queue<EventWrapper> m_event_queue;

      static EventManager s_instance;

      #ifdef SPG_DEBUG
        uint32_t m_max_queue_size = 0;
      #endif

    };
  }

  
}