
#include <string>

struct GLFWwindow;

namespace Spg
{
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
    Window();
    ~Window();
    void Initialise();
    void Shutdown();

    void UpdateSize();
    void ClearBuffers() const;
    void PollEvents() const;
    void SwapBuffers() const;
    void SetVSyncEnabled(bool enable);
    void SetCursorEnabled(bool enable);
    bool IsCursorEnabled() const;
    bool IsVSyncEnabled() const;
    bool IsMinimised() const;
    bool IsMaximised() const;
    float GetAspectRatio() const;
    GLFWwindow* GetWindowHandle() const;
    void MakeContextCurrent() const;
    const Params& GetParams() const;

  private:
    void SetWindowEventCallbacks();
    Params m_params = Params();
    GLFWwindow* m_window_handle = nullptr;
  };
}