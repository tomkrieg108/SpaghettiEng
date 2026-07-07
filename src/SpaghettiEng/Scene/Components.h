#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/matrix_access.hpp> // for glm::column(mat, col_idx)
//#include <glm/gtx/matrix_interpolation.hpp> // Required for axisAngleMatrix
//#include <glm/gtx/euler_angles.hpp> // Required for specific euler extraction seq

#include "MathLib/MathLib.h"

/*

*Note that where a function explicitly specified a sequence, it is mathematically structured as an extrinsic sequence 

glm::eulerAngleXYZ(angleX, angleY, angleZ)
This performs X first, then Y, then Z about the global axes.
R = Rz.Ry.Rx

glm::eulerAngleYXZ(angleY, angleX, angleZ)
This performs Y first, then X, then Z about the global axes.
R = Rz.Rx.Ry

*Dual Interpretation Reminder
As a result of the mathematical duality we discussed, you can comfortably interpret these functions in two completely distinct ways, depending on how you like to think about your game space:

The Extrinsic View (Global Axes): Read the function name from left to right.eulerAngleXYZ means: Rotate about the static world \(X\), then static world \(Y\), then static world \(Z\).

The Intrinsic View (Local Moving Axes): Read the function name from right to left.eulerAngleXYZ means: Rotate about the object's local \(Z\) axis, then its new local \(Y^{\prime }\) axis, then its final local \(X^{\prime \prime }\) axis.

Both mental models result in the exact same final Direction Cosine Matrix (DCM) inside your code.
 
*The standard GLM functions for conversions include:

* Euler -> DCM

Default Extrinsic XYZ Setup:
  glm::mat4 dcm = glm::mat4_cast(glm::quat(glm::vec3(pitch, yaw, roll)));
Explicit Sequence Layouts: (Requires <glm/gtx/euler_angles.hpp>)
  glm::eulerAngleXYZ(pitch, yaw, roll);
  glm::eulerAngleYXZ(yaw, pitch, roll);
  glm::eulerAngleZYX(roll, yaw, pitch); [1] 

* DCM -> Euler
needs  <glm/gtx/euler_angles.hpp>
  float angleX, angleY, angleZ;
  glm::extractEulerAngleXYZ(m_transform, angleX, angleY, angleZ);  

Alternatively
  glm::vec3 euler = glm::eulerAngles(glm::quat_cast(dcm));

*quat -> DCM

Create 4x4 Matrix: glm::mat4 dcm = glm::mat4_cast(myQuat);
Create 3x3 Matrix: glm::mat3 dcm = glm::mat3_cast(myQuat);
Extension Method: glm::toMat4(myQuat); (Requires <glm/gtx/quaternion.hpp>)

*DCM -> quat

From 4x4 Matrix: glm::quat myQuat = glm::quat_cast(matrix4);
From 3x3 Matrix: glm::quat myQuat = glm::quat_cast(matrix3);
Extension Method: glm::toQuat(matrix4); (Requires <glm/gtx/quaternion.hpp>)

*Euler -> Quat

Default Extrinsic XYZ Setup:
glm::quat myQuat = glm::quat(glm::vec3(pitch, yaw, roll));
Custom Sequences Setup: Combine explicit Euler matrix functions with a cast.
glm::quat myQuat = glm::quat_cast(glm::eulerAngleYXZ(yaw, pitch, roll));

*Quat -> Euler

Extract Combined Vector: glm::vec3 euler = glm::eulerAngles(myQuat);
Extract X Angle Only: float pitch = glm::pitch(myQuat);
Extract Y Angle Only: float yaw = glm::yaw(myQuat);
Extract Z Angle Only: float roll = glm::roll(myQuat);

*/

namespace Spg
{
   //* ======================================================
  //* pitch: X, yaw: Y, roll: Z
  //* ======================================================

  
  class StaticTransform
  {
    public:

      StaticTransform() = default;
      StaticTransform(const glm::vec3 position) : m_position(position) {}
      ~StaticTransform() = default;

      glm::vec3 GetPosition()
      {

      }

    private:
      //glm::mat4x3 m_mat; //Todo - this works too - 48 instead of 64 bytes
      glm::mat4 m_transform = glm::mat4(1.0f); //identity mat

      glm::vec3 m_position = glm::vec3(0.0f);
      glm::vec3 m_scale = glm::vec3(1.0f);
      glm::vec3 m_euler_angles = glm::vec3(0.0f);
  };

  // Seperate this out from the transform component
  struct RenderMatrix
  {
    glm::mat4 m_transform = glm::mat4(1.0f);
  };


  class Transform
  {
    public:

      Transform() = default;

      Transform(const glm::vec3 position) : 
        m_position(position),
        m_orientation(1.0f, 0.0f, 0.0f, 0.0f)
      {
      }

      ~Transform() = default;

      void SetPosition(const glm::vec3& position) {
        m_position = position;
        m_dirty = true;
      }

      const glm::vec3& GetPosition() const {
        return m_position;
      }

      void SetScale(const glm::vec3& scale) {
        m_scale = scale;
        m_dirty = true;
      }

      const glm::vec3& GetScale() const {
        return m_scale;
      }

      void SetOrientation(const glm::quat& orientation) {
        m_orientation = orientation;
        m_euler_xyz_deg = glm::degrees(glm::eulerAngles(m_orientation));
        m_dirty = true;
      }

      const glm::quat& GetOrientation() const {
        return m_orientation;
      }

      void SetEuler_xyz(const glm::vec3& euler_xyz_deg) {
        m_euler_xyz_deg = euler_xyz_deg;
        m_orientation = glm::quat(glm::radians(m_euler_xyz_deg));
        m_dirty = true;
      }

      void SetEulerAngles(float pitch_deg, float yaw_deg, float roll_deg) {
        SetEuler_xyz(glm::vec3(pitch_deg,yaw_deg,roll_deg));
      }

      glm::vec3 GetEulerXYZ() const {
        return  m_euler_xyz_deg;
      }

      void Translate(const glm::vec3& delta_pos) {
        m_position += delta_pos;
        m_dirty = true;
      }

      void Rotate(const glm::vec3& axis, float angle_degrees) {
        glm::quat q = glm::angleAxis(glm::radians(angle_degrees), glm::normalize(axis));
        m_orientation = q * m_orientation;
        m_orientation = glm::normalize(m_orientation);
        m_euler_xyz_deg = glm::degrees(glm::eulerAngles(m_orientation));
        m_dirty = true;
      }

      //These are effectively calculating the columns (basis vectors) of the DCM matrix from the quaternion components using eq 2.66 in FGED (no need for the actual 4x4 matrix)
      glm::vec3 LocalRight() {
        return m_orientation * glm::vec3(1.0f, 0.0f, 0.0f);
      }

      glm::vec3 LocalUp() {
        return m_orientation * glm::vec3(0.0f, 1.0f, 0.0f);
      }

      glm::vec3 LocalForward() {
        return m_orientation * glm::vec3(0.0f, 0.0f, -1.0f);
      }

      glm::mat4 GetMatrix() const {
        if(m_dirty) 
        {
          glm::mat4 rot = glm::mat4_cast(m_orientation);
          m_transform = glm::scale(rot, m_scale);
          m_transform[3] = glm::vec4(m_position,1.0f);
          m_dirty = false;  
        }
        return m_transform;
      }


    private:
      //Note: member vars initialized in the arder of declaration, not order in initializer list
      glm::vec3 m_position = glm::vec3(0.0f);
      glm::quat m_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
      glm::vec3 m_scale = glm::vec3(1.0f);
      glm::vec3 m_euler_xyz_deg; //Only for display in inspector

      mutable glm::mat4 m_transform = glm::mat4(1.0f); //identity mat
      mutable bool m_dirty = false; //If true then construct matrix on demand
  };

}