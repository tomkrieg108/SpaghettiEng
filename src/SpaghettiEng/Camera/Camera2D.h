#pragma once
#include <glm/glm.hpp>

namespace Spg
{
  /*
    Cameras model matrix default to Identity, meaning it is looking in -ve z dir.  +X to the right, +Y up.  Objects need be drawn in XY plane and have a z-coord < cameras z value
  */
  class Camera2D
  {
  public:

    struct Params
    {
      float left = -100.0f;
      float right = 100.0f;
      float bottom = -100.0f;
      float top = 100.0f;
    };

  public:
    Camera2D();
    ~Camera2D() = default;

    glm::vec2 GetPosition();
    glm::mat4& GetTransform();
    glm::mat4 GetProjMatrix() const;
    glm::mat4 GetViewMatrix() const;

  private:  
    Params m_params;
    glm::mat4 m_transform = glm::mat4(1.0f);

    friend class CameraController2D;
  };  
}