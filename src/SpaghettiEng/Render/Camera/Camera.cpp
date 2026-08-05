//#include <cmath>	//abs
#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "CoreLib/Logger.h"
#include "SpaghettiEng/Render/Camera/Camera.h"

#include "MathLib/MathLib.h"

namespace Spg
{
  Camera::Camera()
  {
    SetPosition(glm::vec3(5,3,2));
    LookAt(glm::vec3(0,0,0));
  }

  Camera::Camera(CameraType camera_type, float width, float height) :
    m_camera_type{camera_type}
  {
    if(camera_type == CameraType::Perspective)
      SetAspectRatio(width/height);
    else {
      m_ortho_params = {-width,width,-height,height};
    }  

    //Todo - position and orientation as ctr parameters
    SetPosition(glm::vec3(5,3,2));
    LookAt(glm::vec3(0,0,0));
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

  /*
    Todo: Calculate faster by splitting the matrix into product of rotation component and position component. Inverse of rot component is it's transpose.  Inverse of pos component will be negated vals (in col 4)
  */
  glm::mat4 Camera::GetViewMatrix() const
  {
    // glm::vec3 camera_pos   = glm::vec3(5.0f, 3.0f, 2.0f);
    // glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    // glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f, 0.0f);
    // glm::mat4 view = glm::lookAt(camera_pos, camera_target, camera_up);
    // return view;

    return glm::inverse(m_transform);
  }

  void Camera::SetAspectRatio(float aspect_ratio)
  {
    m_persp_params.aspect_ratio = aspect_ratio;
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


  //=================================================================

  //* Controller functions - all taken from gl_app

  void Camera::Zoom(float amount)
  {
    if (m_camera_type == CameraType::Perspective)
    {
        float new_fov = m_persp_params.fov + amount;
        new_fov > 75.0f ? m_persp_params.fov = 75.0f : m_persp_params.fov = new_fov;
        new_fov < 1.0f ? m_persp_params.fov = 1.0f : m_persp_params.fov = new_fov;
    }
  }

  void Camera::SetPosition(const glm::vec3& position)
  {
    m_transform[3] = glm::vec4{ position, 1.0f };
  }

  glm::vec3 Camera::Position()
  {
    glm::vec3 pos = (glm::vec3)m_transform[3];
    return pos;
  }

  glm::vec3 Camera::Front()
  {
    glm::vec3 z = (glm::vec3)m_transform[2];
    return -z;  //camera looks in -ve z dir
  }

  glm::vec3 Camera::Right()
  {
    return (glm::vec3)m_transform[0]; //local x
  }

  glm::vec3 Camera::Up()
  {
    return (glm::vec3)m_transform[1]; //local y
  }

  void Camera::LookAt(const glm::vec3& look_pos)
  {
    glm::vec3 pos = (glm::vec3)m_transform[3];
    glm::vec3 z = -glm::normalize(look_pos - pos); // negative front
    glm::vec3 up = glm::vec3(0, 1, 0);
    // Check if z is parallel to the global up vector
    if (glm::abs(glm::dot(z, up)) > 0.99f) {
      // Choose a different "up" vector to avoid degeneracy
      up = glm::vec3(1, 0, 0); // Global X-axis as a fallback
    }
    glm::vec3 x = glm::normalize(glm::cross(up,z));
    glm::vec3 y = glm::normalize(glm::cross(z, x));
    m_transform[0] = glm::vec4{ x,0.0f };
    m_transform[1] = glm::vec4{ y,0.0f };
    m_transform[2] = glm::vec4{ z,0.0f };
  }

  //When setting up cube map, up vector needs to be in a specific direction depending on the face direction
  //So use this version.  (ChatGPT "Equirectangular Projections)
  void Camera::LookAt(const glm::vec3& look_pos, const glm::vec3& up) 
  {
    glm::vec3 pos = glm::vec3(m_transform[3]);
    glm::vec3 z = -glm::normalize(look_pos - pos); // Negative front
    glm::vec3 x = glm::normalize(glm::cross(up, z));
    glm::vec3 y = glm::normalize(glm::cross(z, x));
    m_transform[0] = glm::vec4{ x, 0.0f };
    m_transform[1] = glm::vec4{ y, 0.0f };
    m_transform[2] = glm::vec4{ z, 0.0f };
  }

  void Camera::InvertXYAxes()
  {
    m_transform[0] = -m_transform[0];
    m_transform[1] = -m_transform[1];
  }

  void Camera::MoveForward(float amount)
  {
    m_transform = glm::translate(m_transform, glm::vec3(0, 0, amount));
  }

  void Camera::MoveRight(float amount)
  {
    m_transform = glm::translate(m_transform, glm::vec3(amount, 0, 0));
  }

  void Camera::MoveVertically(float amount)
  {
    glm::vec3 up = glm::vec3{ glm::inverse(m_transform) * glm::vec4{0,1,0,0} }; //global up in camera space
    m_transform = glm::translate(m_transform, amount * up);
  }

  void Camera::Turn(float delta_yaw, float delta_pitch)
  {
    glm::vec3 camera_front = glm::vec3{ m_transform[2] };
    glm::vec3 world_up = glm::vec3(0, 1, 0);

    float pitch_angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
    pitch_angle = glm::degrees(pitch_angle);
    //angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
    //note that both mouse up and right give pos values - see input.cpp

    glm::mat4 rot_y = GetRotationMatY(-delta_yaw);
    m_transform = rot_y * m_transform;  //rotate about Y world axis

    if (pitch_angle > 175 && delta_pitch > 0)
      return;
    if (pitch_angle < 5 && delta_pitch < 0)
      return;

    glm::mat4 rot_x = GetRotationMatX(delta_pitch);
    m_transform = m_transform * rot_x;  //rotate about X local axis (lool up & down)
  }

  void Camera::RotateLocal(float delta_yaw, float delta_pitch)
  {
    //glm::vec3 camera_front = camera.GetFront(camera_transform);
    glm::vec3 camera_front = glm::vec3{ m_transform[2] };
    glm::vec3 world_up = glm::vec3{ 0, 1, 0 };
    glm::vec4 world_up_4 = glm::vec4{ 0, 1, 0, 0 };

    glm::vec3 rot_axis = glm::vec3{ glm::inverse(m_transform) * world_up_4 }; //global Y Axis in camera space
    m_transform = glm::rotate(m_transform, -delta_yaw, rot_axis);

    //angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
    //note that both mouse up and right give pos values - see input.cpp
    float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
    angle = glm::degrees(angle);
    if (angle > 175 && delta_pitch > 0) return;
    if (angle < 5 && delta_pitch < 0) return;

    glm::mat4 rot_x = GetRotationMatX(delta_pitch);
    m_transform = m_transform * rot_x;  // X Axis in camera space
  }

  void Camera::RotateWorld(float amount_x, float amount_y)
  {
    glm::vec3 camera_front = glm::vec3{ m_transform[2] };
    glm::vec3 world_up = glm::vec3(0, 1, 0);

    float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
    angle = glm::degrees(angle);
    //angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
    //note that both mouse up and right give pos values - see input.cpp

    glm::mat4 rot_y = GetRotationMatY(-amount_x);
    m_transform = rot_y * m_transform;  //orbit about Y global axis

    if (angle > 175 && amount_y > 0)
      return;
    if (angle < 5 && amount_y < 0)
      return;

    //glm::mat4 rot_x = GetRotationMatX(amount_y);
    //camera_transform = camera_transform * rot_x;  //rotate about X local axis
  }

  //==================================================================

  glm::mat4 Camera::GetRotationMatX(float angle_deg) const
  {
    float angle_rads = glm::radians(angle_deg);
    float c = std::cos(angle_rads);
    float s = std::sin(angle_rads);

    glm::mat4 m;
    m[0][0] = 1; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
    m[0][1] = 0; m[1][1] = c; m[2][1] = -s; m[3][1] = 0;
    m[0][2] = 0; m[1][2] = s; m[2][2] = c; m[3][2] = 0;
    m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
    return m;
  }

  glm::mat4 Camera::GetRotationMatY(float angle_deg) const
  {
    float angle_rads = glm::radians(angle_deg);
    float c = std::cos(angle_rads);
    float s = std::sin(angle_rads);

    glm::mat4 m;
    m[0][0] = c; m[1][0] = 0; m[2][0] = s; m[3][0] = 0;
    m[0][1] = 0; m[1][1] = 1; m[2][1] = 0; m[3][1] = 0;
    m[0][2] = -s; m[1][2] = 0; m[2][2] = c; m[3][2] = 0;
    m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
    return m;
  }

  glm::mat4 Camera::GetRotationMatZ(float angle_deg) const
  {
    float angle_rads = glm::radians(angle_deg);
    float c = std::cos(angle_rads);
    float s = std::sin(angle_rads);

    glm::mat4 m;
    m[0][0] = c; m[1][0] = -s; m[2][0] = 0; m[3][0] = 0;
    m[0][1] = s; m[1][1] = c; m[2][1] = 0; m[3][1] = 0;
    m[0][2] = 0; m[1][2] = 0; m[2][2] = 1; m[3][2] = 0;
    m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
    return m;
  }

}