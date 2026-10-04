#include "SpaghettiEng/Math/Transform.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

//For V2 only
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>

// {} []

namespace Spg
{
  inline namespace Transform_V2
  {
    void Transform::SetOrientation(const glm::vec3& euler_angles_xyz_deg)
    {
      glm::vec3 angles = glm::radians(euler_angles_xyz_deg);
      // Create individual quaternions for each axis rotation
      glm::quat qx = glm::angleAxis(angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
      glm::quat qy = glm::angleAxis(angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
      glm::quat qz = glm::angleAxis(angles.z, glm::vec3(0.0f, 0.0f, 1.0f));

      //orientation = qx * qy * qz;
      orientation = qz * qy * qx;
    } 

    void Transform::Translate(const glm::vec3& delta_pos) 
    {
      position += delta_pos;
    }

    void Transform::Turn(float pitch, float yaw)
    {
      glm::quat qx = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
      glm::quat qy = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));

      orientation = qy * orientation * qx;

      orientation = glm::normalize(orientation);
    } 

    void Transform::LookAt(const glm::vec3& look_pos)
    {
      glm::vec3 z = -glm::normalize(look_pos - position); // negative front
      glm::vec3 up = glm::vec3(0, 1, 0);
      // Check if z is parallel to the global up vector
      if (glm::abs(glm::dot(z, up)) > 0.999f) {
        // Choose a different "up" vector to avoid degeneracy
        up = glm::vec3(0, 0, 1); // Global X-axis as a fallback
      }
      glm::vec3 x = glm::normalize(glm::cross(up,z));
      glm::vec3 y = glm::normalize(glm::cross(z, x));

      glm::mat3 m(x,y,z);

      orientation = glm::quat_cast(m);
      //glm::quat q2 = glm::quatLookAt(z, up); Alternatively (does the same as above)
    }

    glm::mat4 Transform::ToMat4() const
    {
      glm::mat4 m = glm::mat4_cast(orientation);
      m[0][0] = scale[0];
      m[1][1] = scale[1];
      m[2][0] = scale[2];
      m[3] = glm::vec4{ position, 1.0f };
      return m;
    }

    glm::mat4 Transform::Inverse()
    {
      return glm::inverse(ToMat4());
    }

    

  }

  //===========================================================================


  namespace Transform_v1
  {
    //* Note: from gl_app - refactor later!
    static glm::mat4 GetRotationMatX(float angle_deg);
    static glm::mat4 GetRotationMatY(float angle_deg);
    static glm::mat4 GetRotationMatZ(float angle_deg);

    void Translate(Transform& transform,  glm::vec3& move)
    {
      transform.matrix = glm::translate(transform.matrix, move);
    }

    void Scale(Transform& transform, const glm::vec3& scale)
    {
      transform.matrix = glm::scale(transform.matrix, scale);
    }

    void Rotate(Transform& transform, float angle_deg, const glm::vec3& axis)
    {
      transform.matrix = glm::rotate(transform.matrix, glm::radians(angle_deg),
              axis);
    }


    glm::vec3 Transform::Position() const
    {
      return (glm::vec3)matrix[3];
    }

    glm::vec3 Transform::Front() const
    {
      glm::vec3 z = (glm::vec3)matrix[2];
      return -z;  //camera looks in -ve z dir
    }

    glm::vec3 Transform::Up() const
    {
      return (glm::vec3)matrix[1]; //local y
    }

    glm::vec3 Transform::Right() const
    {
      return (glm::vec3)matrix[0]; //local x
    }

    /** 
    Note: Calculate faster by splitting the matrix into product of rotation component and position component. Inverse of rot component is it's transpose.  Inverse of pos component will be negated vals (in col 4)
    */
    glm::mat4 Transform::Inverse() const
    {
      // glm::vec3 camera_pos   = glm::vec3(5.0f, 3.0f, 2.0f);
      // glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
      // glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f, 0.0f);
      // glm::mat4 view = glm::lookAt(camera_pos, camera_target, camera_up);
      // return view;
      return glm::inverse(matrix);
    }

    void Transform::InvertXYAxes()
    {
      matrix[0] = -matrix[0];
      matrix[1] = -matrix[1];
    }

    void Transform::SetPosition(const glm::vec3& position)
    {
      matrix[3] = glm::vec4{ position, 1.0f };
    }

    void Transform::LookAt(const glm::vec3& look_pos)
    {
      glm::vec3 pos = (glm::vec3)matrix[3];
      glm::vec3 z = -glm::normalize(look_pos - pos); // negative front
      glm::vec3 up = glm::vec3(0, 1, 0);
      // Check if z is parallel to the global up vector
      if (glm::abs(glm::dot(z, up)) > 0.99f) {
        // Choose a different "up" vector to avoid degeneracy
        up = glm::vec3(1, 0, 0); // Global X-axis as a fallback
      }
      glm::vec3 x = glm::normalize(glm::cross(up,z));
      glm::vec3 y = glm::normalize(glm::cross(z, x));
      matrix[0] = glm::vec4{ x,0.0f };
      matrix[1] = glm::vec4{ y,0.0f };
      matrix[2] = glm::vec4{ z,0.0f };
    }

    //When setting up cube map, up vector needs to be in a specific direction depending on the face direction.  So use this version.  (Ref: ChatGPT "Equirectangular Projections)
    void Transform::LookAt(const glm::vec3& look_pos, const glm::vec3& up)
    {
      glm::vec3 pos = glm::vec3(matrix[3]);
      glm::vec3 z = -glm::normalize(look_pos - pos); // Negative front
      glm::vec3 x = glm::normalize(glm::cross(up, z));
      glm::vec3 y = glm::normalize(glm::cross(z, x));
      matrix[0] = glm::vec4{ x, 0.0f };
      matrix[1] = glm::vec4{ y, 0.0f };
      matrix[2] = glm::vec4{ z, 0.0f };
    }

    void Transform::MoveForward(float amount)
    {
       matrix = glm::translate(matrix, glm::vec3(0, 0, amount));
    }

    void Transform::MoveRight(float amount)
    {
      matrix = glm::translate(matrix, glm::vec3(amount, 0, 0));
    }

    void Transform::MoveVertically(float amount)
    {
      glm::vec3 up = glm::vec3{ glm::inverse(matrix) * glm::vec4{0,1,0,0} }; //global up in camera space
      matrix = glm::translate(matrix, amount * up);
    }

    void Transform::Turn(float delta_yaw, float delta_pitch)
    {
      //glm::vec3 camera_front = camera.GetFront(camera_transform);
      glm::vec3 camera_front = glm::vec3{ matrix[2] };
      glm::vec3 world_up = glm::vec3{ 0, 1, 0 };
      glm::vec4 world_up_4 = glm::vec4{ 0, 1, 0, 0 };

      glm::vec3 rot_axis = glm::vec3{ glm::inverse(matrix) * world_up_4 }; //global Y Axis in camera space
      matrix = glm::rotate(matrix, -delta_yaw, rot_axis);
      //matrix = glm::rotate(matrix, -delta_yaw, world_up); //* wrong!

      //angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
      //note that both mouse up and right give pos values - see input.cpp
      float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
      angle = glm::degrees(angle);
      if (angle > 175 && delta_pitch > 0) return;
      if (angle < 5 && delta_pitch < 0) return;

      glm::mat4 rot_x = GetRotationMatX(delta_pitch);
      matrix = matrix * rot_x;  // X Axis in camera space
    }

    void Transform::RotateWorld(float amount_x, float amount_y)
    {
      glm::vec3 camera_front = glm::vec3{ matrix[2] };
      glm::vec3 world_up = glm::vec3(0, 1, 0);

      float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
      angle = glm::degrees(angle);
      //angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
      //note that both mouse up and right give pos values - see input.cpp

      glm::mat4 rot_y = GetRotationMatY(-amount_x);
      matrix = rot_y * matrix;  //orbit about Y global axis

      if (angle > 175 && amount_y > 0)
        return;
      if (angle < 5 && amount_y < 0)
        return;

      //glm::mat4 rot_x = GetRotationMatX(amount_y);
      //camera_transform = camera_transform * rot_x;  //rotate about X local axis
    }

     //==================================================================

    glm::mat4 GetRotationMatX(float angle_deg)
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

    glm::mat4 GetRotationMatY(float angle_deg)
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

    glm::mat4 GetRotationMatZ(float angle_deg)
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

}