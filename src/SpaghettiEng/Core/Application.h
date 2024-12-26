#pragma once
#include "Base.h" //for Scope

namespace Spg
{
  class Window;
  struct EventWindowClose;
  struct EventKeyPressed;

  class Application
  {
  public:
    Application();
    virtual ~Application() = default;

    void Initialise();
    void Run();
    void Shutdown();

  private:
    void SetEventHandlers();
    void OnWindowClosed(EventWindowClose& e);
    void OnKeyPressed(EventKeyPressed& e);

    Scope<Window> m_window = nullptr;
    bool m_running = true; 
  };

  void EngLibHello();

}
