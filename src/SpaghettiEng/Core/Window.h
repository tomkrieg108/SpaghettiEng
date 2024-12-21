
struct GLFWwindow;

namespace Spg
{
  class Window
  {
  public:
    Window();
    ~Window();
    void Initialise();
    void RenderLoop();
    void Shutdown();

  private:
    void SetWindowEventCallbacks();
    GLFWwindow* m_window_handle = nullptr;
  };
}