#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace Spg
{
 class Input
  {
  public:

    Input(GLFWwindow* glfw_window_handle);
    ~Input() = default;

    void SetMouseFirstMoved();
    bool GetMouseFirstMoved();

    float GetMouseX();
    float GetMouseY();
    glm::vec2 GetMousePosition();
   
    float GetMouseDeltaX(float new_x);
    float GetMouseDeltaY(float new_y);
    glm::vec2 GetMouseDelta(float new_x, float new_y);

    bool IsKeyPressed(int key);
    bool IsMousebuttonPressed(int button);

  private:
    float m_last_x =0, m_last_y =0; //mouse pos prior to the previous mouse move
    bool m_mouse_first_moved =false;
    GLFWwindow* m_glfw_window_handle = nullptr;
  };
}