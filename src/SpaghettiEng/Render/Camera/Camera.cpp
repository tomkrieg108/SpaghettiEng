#include "SpaghettiEng/Render/Camera/Camera.h"

// #define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/matrix_access.hpp>
// #include <glm/gtx/vector_angle.hpp>

#include "CoreLib/Logger.h"


namespace Spg
{
  Camera::Camera(CameraType camera_type, float width, float height) :
    m_camera_type{camera_type}
  {
    if(camera_type == CameraType::Perspective)
      SetAspectRatio(width/height);
    else {
      m_ortho_params = {-width,width,-height,height};
    }  
  }

  //Todo - cache this - don't need to rebuild every call
  glm::mat4 Camera::GetProjMatrix() const
  {
    if(m_camera_type == CameraType::Perspective)
      return PerspectiveMatrix();
    else
      return OrthoMatrix();
  }

  glm::mat4 Camera::GetInverseProjMatrix() const
  {
    return glm::inverse(GetProjMatrix());
  }

  void Camera::SetAspectRatio(float aspect_ratio)
  {
    m_persp_params.aspect_ratio = aspect_ratio;
  }

  void Camera::Zoom(float amount)
  {
    if (m_camera_type == CameraType::Perspective)
    {
        float new_fov = m_persp_params.fov + amount;
        new_fov > 75.0f ? m_persp_params.fov = 75.0f : m_persp_params.fov = new_fov;
        new_fov < 1.0f ? m_persp_params.fov = 1.0f : m_persp_params.fov = new_fov;
    }
  }

  glm::mat4 Camera::OrthoMatrix() const
  {
    glm::mat4 mat = glm::ortho( m_ortho_params.left,
                                m_ortho_params.right,
                                m_ortho_params.bottom,
                                m_ortho_params.top, 
                                m_clip_plane.near, 
                                m_clip_plane.far );
    return mat;
  }

  glm::mat4 Camera::PerspectiveMatrix() const
  {
    glm::mat4 mat = glm::perspective(glm::radians(m_persp_params.fov), 
                                    m_persp_params.aspect_ratio, 
                                    m_clip_plane.near, 
                                    m_clip_plane.far);
    return mat;                                
  }
}