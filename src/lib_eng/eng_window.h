
struct GLFWwindow;

namespace bbl
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