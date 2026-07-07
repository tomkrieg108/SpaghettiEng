
#include "CoreLib/Logger.h"
#include "SpaghettiEng/Render/Camera/Camera.h"

#include <glm/glm.hpp>

#include "MathLib/MathLib.h"

namespace Spg
{
  Camera::Camera(CameraType camera_type, float width, float height) :
    m_camera_type{camera_type}
  {
    if(camera_type == CameraType::Perspective)
      SetAspectRatio(width,height);
    else {
      m_ortho_params = {-width,width,-height,height};
    }  
  }

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

  /*
    Todo: Calculate faster by splitting the matrix into product of rotation component and position component. Inverse of rot component is it's transpose.  Inverse of pos component will be negated vals (in col 4)
  */
  glm::mat4 Camera::GetViewMatrix(const glm::mat4& transform_matrix) const
  {
    return glm::inverse(transform_matrix);
  }

  void Camera::SetAspectRatio(float width, float height)
  {
    SPG_ASSERT(width >=10.0f && height >= 10.0f);
    m_persp_params.aspect_ratio = width/height;
  }

  void Camera::Zoom(float amount)
  {
     //Todo
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