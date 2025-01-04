#pragma once
#include "Base.h" //for Scope
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
    virtual ~Application() = default;

    void Initialise();
    void Run();
    void Shutdown();
    void PushLayer(Layer* layer);
    void PopLayer(Layer* layer);
    Window& GetWindow();

    static Application& Get();
    
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

  void EngLibHello();

}
