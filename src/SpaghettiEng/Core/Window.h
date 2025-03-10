#pragma once
#include <Common/Common.h> 

struct GLFWwindow;

namespace Spg
{
  class Input;
  class GLContext;

  class Window
  {
  public:
    struct Params
    {
      std::string title{ "Spaghetti App" };
      int32_t width = 1200;
      int32_t height = 800;
      int32_t buffer_height = 1200;
      int32_t buffer_width = 800;
      bool vsync_enabled = true;
      bool cursor_enabled = true;
    };

  public:
    Window(const std::string& title = std::string(""));
    ~Window();

    void Clear() const;
    void OnUpdate();
    
    void SetVSyncEnabled(bool enable);
    void SetCursorEnabled(bool enable);
    const Params& GetParams() const;
    Params& GetParams();
    Input* GetInput();

    bool IsCursorEnabled() const;
    bool IsVSyncEnabled() const;
    bool IsMinimised() const;
    float GetAspectRatio() const;
    GLFWwindow* GetWindowHandle() const;

  private:
    void SetWindowEventCallbacks();
    
  private:  
    Params m_params = Params();
    GLFWwindow* m_window_handle = nullptr;
    GLContext* m_graphics_context = nullptr; //Todo. Scope<GLContext> ?
    Input* m_input = nullptr; //Todo. Scope<GLContext> ?

    static uint32_t s_window_count;
  };
}