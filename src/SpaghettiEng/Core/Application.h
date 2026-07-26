#pragma once

#include "CoreLib/Core.h"

#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/Layer.h"
#include "SpaghettiEng/Core/ServiceLocator.h"

#include "SpaghettiEng/Resource/ResourceManager.h"

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

    static Application* Instance() {return s_instance;}

    static void Init();

    static void PrintPlatformInfo();
    static void PrintExternalLibInfo();

  private:

    void SetAssetsPath();

    void OnWindowClosed(WinEvt::WindowClose& e);
    void OnKeyPressed(WinEvt::KeyPressed& e);

  protected:
    ServiceLocator m_service_locator;
    LayerStack m_layer_stack;
    //ResourceManager m_resource_manager;

    std::string m_app_name;
    bool m_running = true;


    static Application* s_instance;
  };

  Application* CreateApplication(); //define in client
}
