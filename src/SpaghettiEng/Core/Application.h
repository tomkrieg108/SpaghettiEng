#pragma once

#include "CoreLib/Core.h"

#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/Layer.h"
#include "SpaghettiEng/Core/ServiceLocator.h"

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

    void OnWindowsEvent(WinEvt::Event& event);

    static void SystemInit();
    static Application* Instance() {return s_instance;}

    static void PrintPlatformInfo();
    static void PrintExternalLibInfo();

  private:

    void SetAssetsPath();
   
    void OnWindowClosed(WinEvt::WindowClose& e);
    void OnKeyPressed(WinEvt::KeyPressed& e);

    void OnMouseBtnPressed(WinEvt::MouseBtnPressed e);
  
  protected:
    ServiceLocator m_service_locator;
    LayerStack m_layer_stack;
    std::string m_app_name;
    bool m_running = true;
    static Application* s_instance;
  };

  Application* CreateApplication(); //define in client
}
