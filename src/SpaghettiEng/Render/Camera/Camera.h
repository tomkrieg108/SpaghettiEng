#pragma once

#include <type_traits>

#include <glm/glm.hpp>

#include "MathLib/MathLib.h"

// {} []

//* Note: This Camera class currently includes it's transform matrix and controller functions - refactor later!

namespace Spg
{
  class Camera
  {
  public:

    enum class CameraType { Perspective = 0, Ortho = 1,  };
    
    struct PerspectiveParams
    {
      float aspect_ratio = 1.0f;
      float fov = 60.0f; 
    };

    struct OrthoParams
    {
      float left = -20.0f;
      float right = 20.0f;
      float bottom = -20.0f;
      float top = 20.0f;
    };

    struct ClipPlane
    {
      float near = 0.1f;
      float far = 100.0f;
    };

  public:

    Camera();
    Camera(CameraType camera_type, float width, float height);
    //~Camera() = default;

    glm::mat4 GetProjMatrix() const;
    glm::mat4 GetInverseProjMatrix() const;
    glm::mat4 GetViewMatrix() const;
    void SetAspectRatio(float aspect_ratio);
    

    void SetCameraType(CameraType camera_type) { m_camera_type = camera_type; }
    CameraType SetCameraType() const {return m_camera_type;  }

    void SetPerspectiveParams(const PerspectiveParams& persp_params);
    void SetOrthoParams(const OrthoParams& ortho_params);

    const auto& GetPerspectiveParams() const { return m_persp_params; }
    const auto& GetOrthoParams() const { return m_ortho_params; }

    //* Transform will be in a separate component? - refactor later
    glm::mat4& GetTransform() {return m_transform;} 
    const glm::mat4& GetTransform() const {return m_transform;} 

    //* Controller functions taken from gl_app - refactor later
    void Zoom(float amount);
    glm::vec3 Position();
    glm::vec3 Front();
    glm::vec3 Up();
    glm::vec3 Right();
    void InvertXYAxes();

    void SetPosition(const glm::vec3& position);
    void LookAt(const glm::vec3& look_pos);
    void LookAt(const glm::vec3& look_pos, const glm::vec3& up);
    void MoveForward(float amount);
    void MoveRight(float amount);
    void MoveVertically(float amount);
    void Turn(float delta_yaw, float delta_pitch);
    void RotateLocal(float delta_yaw, float delta_pitch);
    void RotateWorld(float amount_x, float amount_y);


  private:

      glm::mat4 OrthoMatrix() const;
      glm::mat4 PerspectiveMatrix() const;

      CameraType m_camera_type = CameraType::Perspective;
      PerspectiveParams m_persp_params;
      OrthoParams m_ortho_params;
      ClipPlane m_clip_plane;

      //* Transform will be in a separate component? - refactor later
      glm::mat4 m_transform = glm::mat4(1.0f);

      //* Note: from gl_app - refactor later!
      glm::mat4 GetRotationMatX(float angle_deg) const;
      glm::mat4 GetRotationMatY(float angle_deg) const;
      glm::mat4 GetRotationMatZ(float angle_deg) const;
  };

  static_assert(std::is_trivially_copyable_v<Camera>,"Camera class not trivially copiable");
}