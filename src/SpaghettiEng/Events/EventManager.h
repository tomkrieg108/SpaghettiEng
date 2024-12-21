#pragma once

#include "Events.h"

namespace Spg
{
  inline namespace Evt_1
  {
    //This is what Hazel eng does for to define a handler
    #define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

    template<typename T, typename E>
    using MemberFuncType = void(T::*)(E);

    template<typename E>
    using FreeFuncType =  void(*)(E);

    template<typename E>
    class EventHandler
    {
    public:
      virtual ~EventHandler() = default;
      virtual void Invoke(E event) = 0;
    };

    template<typename E>
    class EventHandlerFreeFunc : public EventHandler
    {
    public:
      EventHandlerFreeFunc(FreeFuncType<E> handler) : m_handler{handler} {}

      void Invoke(E event) override
      {
        m_handler(event);
      }

    private:
        FreeFuncType m_handler;
    };

    template<typename T, typename E>
    class EventHandlerMemberFunc : public EventHandler
    {
    public:
      EventHandlerMemberFunc(T* instance, MemberFuncType<T,E> handler) : 
        m_instance{instance}
        m_handler{handler} 
      {}

      void Invoke(E event) override
      {
        m_instance->*m_handler(event);
      }

    private:
      T* m_instance;
      MemberFuncType m_handler;
    };


    class EventManager
    {
    public:
      template<typename T, typename E>
      static void AddHandler(T* instance, MemberFuncType<T,E> handler);

       template<typename E>
      static void AddHandler(FreeFuncType<E> handler);

    private: 

    };

  }

  namespace Evt_2
  {

  }

  
}