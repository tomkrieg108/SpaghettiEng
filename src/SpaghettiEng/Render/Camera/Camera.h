#pragma once

#include <type_traits>

#include <glm/mat4x4.hpp>

// {} []

namespace Spg
{
  class Camera
  {
  public:

    enum class CameraType {Perspective, Ortho};
    
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

    Camera() = default;
    Camera(CameraType camera_type, float width, float height);
    
    glm::mat4 GetProjMatrix() const;
    glm::mat4 GetInverseProjMatrix() const;
    void SetAspectRatio(float aspect_ratio);
    void Zoom(float amount);
    void SetCameraType(CameraType camera_type) { m_camera_type = camera_type; }
    CameraType GetCameraType() const {return m_camera_type;  }
    void SetPerspectiveParams(const PerspectiveParams& persp_params) { 
      m_persp_params = persp_params; 
    }
    void SetOrthoParams(const OrthoParams& ortho_params){ 
      m_ortho_params = ortho_params; 
    }
    const auto& GetPerspectiveParams() const { return m_persp_params; }
    const auto& GetOrthoParams() const { return m_ortho_params; }

  private:

      glm::mat4 OrthoMatrix() const;
      glm::mat4 PerspectiveMatrix() const;

      CameraType m_camera_type = CameraType::Perspective;
      PerspectiveParams m_persp_params;
      OrthoParams m_ortho_params;
      ClipPlane m_clip_plane;
  };

  static_assert(std::is_trivially_copyable_v<Camera>,"Camera class not trivially copiable");
}