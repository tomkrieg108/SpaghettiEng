
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Application.h"
#include "Input.h"

namespace Spg
{
  float Input::m_last_x = 0;
  float Input::m_last_y = 0;
  bool  Input::m_mouse_first_moved = false;
    
  float Input::GetMouseX()
  {
    return GetMousePosition().x;
  }

  float Input::GetMouseY()
  {
    return GetMousePosition().y;
  }

  glm::vec2 Input::GetMousePosition()
  {
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindowHandle());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return { (float)xpos, (float)ypos };
  }

  bool Input::IsKeyPressed(int key)
  {
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindowHandle());
    auto state = glfwGetKey(window, static_cast<int32_t>(key));
    return state == GLFW_PRESS;
  }

  bool Input::IsMousebuttonPressed(int button)
  {
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindowHandle());
    auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
    return state == GLFW_PRESS;
  }

  void Input::SetMouseFirstMoved()
  {
    m_mouse_first_moved = true;
    m_last_x = GetMousePosition().x;
    m_last_y = GetMousePosition().y;
  }

  bool Input::GetMouseFirstMoved()
  {
    return m_mouse_first_moved;
  }

  float Input::GetMouseDeltaX(float new_x)
  { 
    float delta_x = new_x - m_last_x;
    m_last_x = new_x;
    return delta_x;
  }

  float Input::GetMouseDeltaY(float new_y)
  { 
    float delta_y = m_last_y - new_y;
    m_last_y = new_y;
    return delta_y;
  }

  glm::vec2 Input::GetMouseDelta(float new_x, float new_y)
  {
    //top left is (0,0)
    //mouse up & mouse right give pos deltas with this
    float delta_x = new_x - m_last_x;
    float delta_y = m_last_y - new_y;
    m_last_x = new_x;
    m_last_y = new_y;
    return glm::vec2{ delta_x , delta_y };
  }
}