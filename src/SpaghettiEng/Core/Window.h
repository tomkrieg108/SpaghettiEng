#pragma once
//#include "CoreLib/Core.h"

struct GLFWwindow; //glfw lib window

namespace Spg
{
  class InputState;
  class OpenGLContext;

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
    InputState* GetInputState();
    OpenGLContext* GetGraphicsContext();

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
    OpenGLContext* m_graphics_context = nullptr; 
    InputState* m_input_state = nullptr; 

    static uint32_t s_window_count;
  };
}