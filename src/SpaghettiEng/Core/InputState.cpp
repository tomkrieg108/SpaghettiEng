
#include "SpaghettiEng/Core/InputState.h"

#include <GLFW/glfw3.h>

#include "CoreLib/Core.h"
#include "Window.h"

namespace Spg
{
  
  InputState::InputState(GLFWwindow* glfw_window_handle) :
    m_glfw_window_handle{glfw_window_handle}
  {
    SPG_ASSERT(m_glfw_window_handle != nullptr)   ; 
  }

  float InputState::GetMouseX()
  {
    return GetMousePosition().x;
  }

  float InputState::GetMouseY()
  {
    return GetMousePosition().y;
  }

  glm::vec2 InputState::GetMousePosition()
  {
    double xpos, ypos;
    glfwGetCursorPos(m_glfw_window_handle, &xpos, &ypos);
    return { (float)xpos, (float)ypos };
  }

  bool InputState::IsKeyPressed(int key)
  {
    auto state = glfwGetKey(m_glfw_window_handle, key);
    return state == GLFW_PRESS;
  }

  bool InputState::IsMousebuttonPressed(int button)
  {
    auto state = glfwGetMouseButton(m_glfw_window_handle, button);
    return state == GLFW_PRESS;
  }

  void InputState::SetMouseFirstMoved()
  {
    m_mouse_first_moved = true;
    m_last_x = GetMousePosition().x;
    m_last_y = GetMousePosition().y;
  }

  bool InputState::GetMouseFirstMoved()
  {
    return m_mouse_first_moved;
  }

  float InputState::GetMouseDeltaX(float new_x)
  { 
    float delta_x = new_x - m_last_x;
    m_last_x = new_x;
    return delta_x;
  }

  float InputState::GetMouseDeltaY(float new_y)
  { 
    float delta_y = m_last_y - new_y;
    m_last_y = new_y;
    return delta_y;
  }

  glm::vec2 InputState::GetMouseDelta(float new_x, float new_y)
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