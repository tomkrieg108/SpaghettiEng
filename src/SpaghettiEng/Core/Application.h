#pragma once

#include <Common/Common.h>
#include "Events/Events.h"
#include "Layer.h"
#include "AppContext.h"

namespace Spg
{
  class Application
  {
  public:
    Application(const std::string& app_name = std::string{"Spaghetti App"});
    virtual ~Application();

    static void SystemInit();

    void Run();
    void PushLayer(Layer* layer);
    void PopLayer(Layer* layer);
   
  private:
    void SetEventHandlers();
    void SetAssetsPath();
    void OnWindowClosed(EventWindowClose& e);
    void OnKeyPressed(EventKeyPressed& e);
  
  protected:

    AppContext m_app_context;
    LayerStack m_layer_stack;
    std::string m_app_name;
    bool m_running = true;
    static Application* s_instance;
  };

  Application* CreateApplication(); //defined in client
}
