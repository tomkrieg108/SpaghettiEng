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
    Application(const std::string& title = "Spaghetti App");
    virtual ~Application();

    void Run();
    void PushLayer(Layer* layer);
    void PopLayer(Layer* layer);
   
  private:
    void InitialiseAppContext(const std::string& title);
    void SetEventHandlers();
    void SetAssetsPath();
    void OnWindowClosed(EventWindowClose& e);
    void OnKeyPressed(EventKeyPressed& e);
  
  protected:
    AppContext m_app_context;
    LayerStack m_layer_stack;

    static Application* s_instance;
  };

  Application* CreateApplication(); //defined in client
}
