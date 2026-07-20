#pragma once

#include "CoreLib/Core.h"
#include "Events.h"

namespace Spg
{
 
 inline namespace Evt_2
  {

    //This is what Hazel eng does for to define a handler
    //#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

    template<typename T, typename E>
    using MemberFuncType = void(T::*)(E&);

    template<typename E>
    using FreeFuncType =  void(*)(E&);

    class EventHandler
    {
    public:
      virtual ~EventHandler() = default;
      virtual void Handle(Event& event) = 0;
    };

    template<typename E>
    class EventHandlerFreeFunc : public EventHandler
    {
    public:
      EventHandlerFreeFunc(FreeFuncType<E> handler) : m_handler{handler} {}
      void Handle(Event& event) override { m_handler( static_cast<E&>(event) );}

    private:
      FreeFuncType<E> m_handler;
    };

    template<typename T, typename E>
    class EventHandlerMemberFunc : public EventHandler
    {
    public:
      EventHandlerMemberFunc(T* instance, MemberFuncType<T,E> handler) : m_instance{instance},m_handler{handler} {}
      void Handle(Event& event) override { (m_instance->*m_handler)( static_cast<E&>(event) );}
    private:
      T* m_instance;
      MemberFuncType<T,E> m_handler;
    };

    
  //------------------------------------------
    class EventManager
    {

      using WindowEvent = std::variant<EventWindowClose, EventWindowResize, EventMouseButtonPressed, EventMouseButtonReleased, EventMouseMoved, EventMouseScrolled, EventKeyPressed, EventKeyReleased>;

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

      //static void Initialise();
      static void HandleQueuedEvents();

      // template<typename E>
      // static void Handle(E& event)
      // {
      //   static_assert(std::derived_from<E, Event>);
      //   uint32_t type = (uint32_t)(E::GetStaticType());
      //   auto handler = s_instance.m_handler_list[type];  
      //   SPG_ASSERT(handler != nullptr);
      //   handler->Handle(event);
      // }

      template<typename E>
      static void Dispatch(E& event)
      {
        static_assert(std::derived_from<E, Event>);
        s_instance.m_event_queue.push({E::GetStaticType(), event});
        //s_instance.m_event_queue.push({event});

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
    // Todo - constrained to a single event handler per event
      std::array<EventHandler*, (size_t)EventType::Count> m_handler_list;
      std::queue<EventWrapper> m_event_queue;

      static EventManager s_instance;

      #ifdef SPG_DEBUG
        uint32_t m_max_queue_size = 0;
      #endif

    };

    namespace AI
    {
      // 1. The base class has NO virtual functions (zero vtable overhead)
        class EventHandlerBase {
        protected:
            EventHandlerBase() = default;
        };

        // 2. The derived class remains exactly as you envisioned
        template<typename T, typename E>
        class EventHandlerMemberFunc2 : public EventHandlerBase {
        public:
            using MemberFuncType = void(T::*)(E&);
            
            EventHandlerMemberFunc2(T* instance, MemberFuncType handler) 
                : m_instance{instance}, m_handler{handler} {}

            // Clean, direct, non-virtual function
            void HandleDirect(E& event) { 
                (m_instance->*m_handler)(event); 
            }

        private:
            T* m_instance;
            MemberFuncType m_handler;
        };

        /*
          std::visit([&](auto&& event) {
                // 1. Get the raw base pointer
                EventHandlerBase* base_handler = s_instance.m_handler_list[type];
                SPG_ASSERT(base_handler != nullptr);

                // 2. Deduce the exact concrete type of the handler at compile-time
                using ConcreteEvent = std::decay_t<decltype(event)>;
                using ConcreteHandler = EventHandlerMemberFunc2<SubsystemType, ConcreteEvent>;

                // 3. Static cast directly to the derived type (Zero runtime cost)
                auto* handler = static_cast<ConcreteHandler*>(base_handler);

                // 4. Direct invoke of the member function pointer
                handler->HandleDirect(event);

            }, this_event);
        
        */

        /*
          Why this achieves your goal:No Virtual Dispatch: HandleDirect is not virtual. The CPU does not look up a vtable. It reads the member function pointer and jumps directly to it.No Dynamic Casting: Because std::visit ensures that the event matches the handler's registered event type E, a static_cast is perfectly safe and compiles down to exactly zero CPU instructions.Pointers Allowed: Your m_handler_list remains a clean, standard array of simple pointers.The only constraint here is SubsystemType in step 2. For this exact syntax to compile, all your handlers must belong to the same class type, or you must know the class type at that point.
        */

        /*
        To support event handlers scattered across completely different classes without virtual functions, downcasting, or custom template classes, you can replace your custom structure with an array of std::move_only_function (C++23) or std::function (C++11).This pattern uses a type-erased lambda wrapper to instantly capture the concrete system pointer and member function pointer at compile time.Here is how you can update your event system to achieve peak efficiency with zero virtual dispatch.
        
        1. Update the Handler StorageInstead of storing custom base class pointers, store uniform, callable function objects that accept the generic Event& base class.

        #include <functional> // For std::function (or <move_only_function> in C++23)
        #include <vector>

        class EventSystem {
        private:
            // A uniform array of callables. No custom inheritance needed.
            // Use std::move_only_function<void(Event&)> in C++23 for even better performance.
            std::vector<std::function<void(Event&)>> m_handler_list;
        };

        2. Update the Registration FunctionWhen an external system registers, you write a template function. This function creates a lambda wrapper that locks in the exact type of the subsystem (T) and the exact type of the event (E) at compile time.

        template <typename T, typename E>
          void RegisterHandler(uint32_t eventType, T* instance, void(T::*memberFunc)(E&)) {
              
              // Resize the array to fit the event ID if necessary
              if (eventType >= m_handler_list.size()) {
                  m_handler_list.resize(eventType + 1);
              }

              // Capture the exact system and member function pointers inside a lambda.
              // The lambda accepts a generic Event& to satisfy the std::function interface,
              // but executes a completely safe static_cast because we know the exact event type.
              m_handler_list[eventType] = [instance, memberFunc](Event& base_event) {
                  // 1. Static cast the event directly to its true concrete type (Zero runtime cost)
                  E& concrete_event = static_cast<E&>(base_event);
                  
                  // 2. Direct, standard call to the member function (No virtual dispatch)
                  (instance->*memberFunc)(concrete_event);
              };
          }

          3. Update the std::visit Dispatch LoopYour dispatch loop now becomes incredibly simple and lightweight. std::visit ensures your variant data is accessed safely, and the lambda array instantly forwards it to the correct member function.

          void Dispatch(std::variant<EventA, EventB, EventC>& this_event, uint32_t type) {
            // 1. Check if a handler exists for this type
            if (type >= m_handler_list.size() || !m_handler_list[type]) {
                return; 
            }

            // 2. Visit the variant to extract the concrete underlying event reference
            std::visit([&](auto&& concrete_event) {
                
                // 3. Invoke the registered lambda wrapper.
                // This implicitly casts the concrete event back to the base Event& reference
                // required by the std::function signature.
                m_handler_list[type](concrete_event);

            }, this_event);
          }

          Why This is Significantly FasterZero Virtual Tables: Neither your event handlers nor your Event classes need virtual function tables. The CPU completely avoids the standard vtable lookup overhead.Optimized Compiler Inlining: Inside the RegisterHandler lambda, the compiler explicitly knows both T and E. It can directly inline the member function jump right into the lambda's execution block.Hardware-Friendly Dispatch: At runtime, execution flows directly from the std::visit switch block straight into the single function pointer stored in m_handler_list[type]. The double-indirection bottleneck is thoroughly eliminated.
          
          Would you like to see how to adapt this setup if an event type is allowed to have multiple listeners registered to it at the same time, or how to handle rvalue events (moving data) safely through this pipeline?

        */

        /*
          Overload pattern:

          // The pattern definition
          template<class... Ts> struct overload : Ts... { using Ts::operator()...; };

          int main() {
              std::variant<int, float, std::string> myVariant = "Hello World";

              std::visit(overload {
                  [](int i) { std::cout << "It's an int: " << i << '\n'; },
                  [](float f) { std::cout << "It's a float: " << f << '\n'; },
                  [](const std::string& s) { std::cout << "It's a string: " << s << '\n'; }
              }, myVariant);
          }

          Can also do (effectively the same as above):

           std::visit([](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>)
                std::cout << "int with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, long>)
                std::cout << "long with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, double>)
                std::cout << "double with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, std::string>)
                std::cout << "std::string with value " << std::quoted(arg) << '\n';
            else
                static_assert(false, "non-exhaustive visitor!");
        }, w);
        
        */


    }
  }

  
}