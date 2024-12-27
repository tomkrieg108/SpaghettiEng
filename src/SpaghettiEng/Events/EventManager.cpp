#include "EventManager.h"

namespace Spg
{
 inline namespace Evt_2
  {
    EventManager EventManager::s_instance;

    static void DefaultHandler(Event& e)
    {
      std::cout << "Default event callback Events V2\n";
    }

    EventManager::EventManager()
    {
      for(auto i=0; i<m_handler_list.size(); ++i)
      {
        EventHandler* handler = new EventHandlerFreeFunc(DefaultHandler); 
        m_handler_list[i] = handler;
      }
    } 

    void EventManager::Initialise()
    {
       SPG_INFO("Initialising Event Manager V2");
    }

    void EventManager::DispatchQueuedEvents()
    {
      while (!s_instance.m_event_queue.empty())
      {
        auto& this_event = s_instance.m_event_queue.front().event;
        uint32_t type = (uint32_t)s_instance.m_event_queue.front().type;   

        // Use std::visit to dynamically dispatch to the appropriate handler
        // std::visit() - compiler generates a specific implementation for each type that the variant con hold
        std::visit([&](auto&& event) {
           auto handler = s_instance.m_handler_list[type];   
           SPG_ASSERT(handler != nullptr);
           handler->Dispatch(event);
        },this_event);

        s_instance.m_event_queue.pop();
      }
    }

  }
}