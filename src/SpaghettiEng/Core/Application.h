#pragma once

#include <Common/Common.h>
#include "Events/Events.h"
#include "Layer.h"
#include "Window.h" 

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
    Window& GetWindow();
    Utils::SpdLogger GetLogger();

    static Application& Get();
    
  private:
    void SetEventHandlers();
    void SetAssetsPath();
    void OnWindowClosed(EventWindowClose& e);
    void OnKeyPressed(EventKeyPressed& e);
    void ImGuiAppRender();

  private:
    Scope<Window> m_window = nullptr;
    LayerStack m_layer_stack;
    std::string m_app_title{""};
    Utils::SpdLogger m_log;
    bool m_running = true;

    static Application* s_instance;
  };

  Application* CreateApplication(); //defined in client

  void EngLibHello();
}

#define SPG_TRACE(...)  LOG_TRACE(Application::Get().GetLogger(), __VA_ARGS__)
#define SPG_INFO(...)  LOG_INFO(Application::Get().GetLogger(), __VA_ARGS__)
#define SPG_WARN(...)  LOG_WARN(Application::Get().GetLogger(), __VA_ARGS__)
#define SPG_ERROR(...)  LOG_ERROR(Application::Get().GetLogger(), __VA_ARGS__)
#define SPG_CRITICAL(...)  LOG_CRITICAL(Application::Get().GetLogger(), __VA_ARGS__)
