
struct GLFWwindow;

namespace spg
{
  class Window
  {
  public:
    Window();
    ~Window();
    void Init();
    void RenderLoop();

  private:
    GLFWwindow* m_handle = nullptr;
  };
}