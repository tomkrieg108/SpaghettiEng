#pragma once
//#include "Base.h"
#include <Common/Common.h>
#include "Layer.h"

namespace Spg
{
  class Window;
  struct EventWindowClose;
  struct EventKeyPressed;

  class Application
  {
  public:
    Application(const std::string& title = "Spaghetti App");
    virtual ~Application();

    void Run();
    void PushLayer(Layer* layer);
    void PopLayer(Layer* layer);
    Window& GetWindow();

    static Application& Get();
    Utils::SpdLogger m_log;
    
  private:
    void SetEventHandlers();
    void OnWindowClosed(EventWindowClose& e);
    void OnKeyPressed(EventKeyPressed& e);
    void ImGuiAppRender();

  private:
    Scope<Window> m_window = nullptr;
    LayerStack m_layer_stack;
    bool m_running = true;
    std::string m_app_title = std::string("");
    static Application* s_instance;
  };

  Application* CreateApplication(); //defined in client

  void EngLibHello();
}

#define SPG_TRACE(...)  LOG_TRACE(Application::Get().m_log, __VA_ARGS__)
#define SPG_INFO(...)  LOG_INFO(Application::Get().m_log, __VA_ARGS__)
#define SPG_WARN(...)  LOG_WARN(Application::Get().m_log, __VA_ARGS__)
#define SPG_ERROR(...)  LOG_ERROR(Application::Get().m_log, __VA_ARGS__)
#define SPG_CRITICAL(...)  LOG_CRITICAL(Application::Get().m_log, __VA_ARGS__)
