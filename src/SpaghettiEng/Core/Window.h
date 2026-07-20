#pragma once

#include <functional>

struct GLFWwindow; //glfw lib window

namespace Spg
{
  class InputState;
  class OpenGLContext;
  class MemberCallback;
  
  namespace WinEvt
  {
    struct Event;
    using Callback = std::function<void(Event&)>;
  }
  // {}
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
    bool ShouldClose();
    
    void SetVSyncEnabled(bool enable);
    void SetCursorEnabled(bool enable);
    bool IsMinimised() const;
    float GetAspectRatio() const;

    GLFWwindow* GetWindowHandle() const { return m_window_handle; }
    const Params& GetParams() const { return m_params; }
    Params& GetParams() { return m_params; };
    InputState* GetInputState() const { return m_input_state;  }  
    OpenGLContext* GetGraphicsContext() const { return m_graphics_context; }

    bool IsCursorEnabled() const { return m_params.cursor_enabled;  }
    bool IsVSyncEnabled() const  { return m_params.vsync_enabled; }
    
    void SetEventCallback(const WinEvt::Callback& callback) { m_event_callback = callback;}
    WinEvt::Callback& GetEventCallback() { return m_event_callback; }
   
  private:
    void SetWindowEventCallbacks();
    
  private:  
    Params m_params = Params();

    GLFWwindow* m_window_handle = nullptr; //has to be a pointer
    OpenGLContext* m_graphics_context = nullptr; // pointer, since maybe an interface later
    InputState* m_input_state; //could be instance!
    WinEvt::Callback m_event_callback;
    
    static uint32_t s_window_count;
  };
}