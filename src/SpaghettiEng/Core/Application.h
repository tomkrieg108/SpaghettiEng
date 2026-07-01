#pragma once

#include "CoreLib/Core.h"
#include "Events/Events.h"
#include "Layer.h"
#include "ServiceLocator.h"

namespace Spg
{
  class Application
  {
  public:
    Application(const std::string& app_name = std::string{"Spaghetti App"});
    virtual ~Application();

    void Run();
    void PushLayer(Layer* layer);
    void PopLayer(Layer* layer);

    static void SystemInit();
    static Application* Instance();

    static void PrintPlatformInfo();
    static void PrintExternalLibInfo();

  private:
    void SetEventHandlers();
    void SetAssetsPath();
    void OnWindowClosed(EventWindowClose& e);
    void OnKeyPressed(EventKeyPressed& e);
  
  protected:

    ServiceLocator m_service_locator;
    LayerStack m_layer_stack;
    std::string m_app_name;
    bool m_running = true;
    static Application* s_instance;
  };

  Application* CreateApplication(); //defined in client
}
