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
      float left = -20.0f;
      float right = 20.0f;
      float bottom = -20.0f;
      float top = 20.0f;
      float z_near = -20.0f;
      float z_far = 20.0f;
    };

  public:
    Camera2D();
    ~Camera2D() = default;

    glm::vec2 GetPosition() const;
    glm::mat4 GetProjMatrix() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4& GetTransform();

    Camera2D::Params GetParams() const;
    void SetParams(const Camera2D::Params& params);

    glm::vec3 Front() const;
    glm::vec3 Up() const;
    glm::vec3 Right() const;

    //Todo - these in controller class?
    void SetPosition(glm::vec3& position);
    void LookAt(glm::vec3& look_pos);
    void LookAt(glm::vec3& look_pos, glm::vec3& up);

  private:  
    Params m_params;
    glm::mat4 m_transform = glm::mat4(1.0f);

    friend class CameraController2D;
  };  
}