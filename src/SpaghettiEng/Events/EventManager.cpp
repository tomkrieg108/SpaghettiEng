
#include "EventManager.h"

namespace Spg
{
 inline namespace Evt_2
  {
    EventManager EventManager::s_instance;

    static void DefaultHandler(Event& e)
    {
    }

    EventManager::EventManager()
    {
      for(auto i=0; i<m_handler_list.size(); ++i)
      {
        //Todo - memory leak - default handlers allocated here, then overwritten by AddHandler()
        EventHandler* handler = new EventHandlerFreeFunc(DefaultHandler); 
        m_handler_list[i] = handler;
      }
    } 

    // void EventManager::Initialise()
    // {
    //   //Todo - what's this!?
    // }

    void EventManager::HandleQueuedEvents()
    {
      while (!s_instance.m_event_queue.empty())
      {
        auto& this_event = s_instance.m_event_queue.front().event; //an EvenVariant, not Event
        uint32_t type = (uint32_t)s_instance.m_event_queue.front().type;   
       
        // Use std::visit to dynamically dispatch to the appropriate handler
        // std::visit() - compiler generates a specific implementation for each type that the variant con hold
        // generates a vtable to call the function associated with the actual variant type O(1).  Almost no overhead after compiler optimization if just fowarding the arg to anythe function 
        std::visit([&](auto&& event) {
           auto handler = s_instance.m_handler_list[type];   
           SPG_ASSERT(handler != nullptr);
           handler->Handle(event);
        },this_event);

        s_instance.m_event_queue.pop();
      }
    }

  }
}