#pragma once

#include <glm/glm.hpp>

//Takes straign from Elysium
namespace Spg
{
 class Input
  {
  public:

    static void SetMouseFirstMoved();
    static bool GetMouseFirstMoved();

    static float GetMouseX();
    static float GetMouseY();
    static glm::vec2 GetMousePosition();
   
    static float GetMouseDeltaX(float new_x);
    static float GetMouseDeltaY(float new_y);
    static glm::vec2 GetMouseDelta(float new_x, float new_y);

    static bool IsKeyPressed(int key);
    static bool IsMousebuttonPressed(int button);

  private:
    static float m_last_x, m_last_y; //mouse pos prior to the previous mouse move
    static bool m_mouse_first_moved;
  };
}