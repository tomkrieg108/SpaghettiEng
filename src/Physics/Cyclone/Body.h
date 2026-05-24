#pragma once

#include "Math/MathX.h"
#include "Common/Common.h"

namespace Cyc
{
  using namespace MathX;

  class RigidBidy
  {

  protected:
    MathX::Real m_inverse_mass;
    MathX::Mat3 m_inverse_inertia_tensor; // In body space
    MathX::Real m_linear_damping;
    MathX::Real m_angular_damping;
    MathX::Vec3 m_position; //world space
    MathX::Quat m_orientation; // angular orientation in world space
    MathX::Vec3 m_velocity; //world space
    MathX::Vec3 m_rotation; //angular velocity in world space
    MathX::Mat3 m_inverse_inertia_tensor_world;
    MathX::Real m_motion; // use to put a body to 'sleep'
    bool m_is_awake;
    bool m_can_sleep;
    MathX::Mat4 m_transform_matrix; // body to world
    MathX::Vec3 m_force_accumulation;
    MathX::Vec3 m_torque_accumulation;
    MathX::Vec3 m_acceleration;
    MathX::Vec3 m_last_frame_acceleration;

  public:
    void CalculateDerivedData();
    void Integrate(Real time_step);
    void SetMass(const Real mass);
    Real GetMass() const;
    void SetInverseMass(const Real inverse_mass);
    Real GetInverseMass() const;
    bool HasFiniteMass() const;
    void setInertiaTensor(const MathX::Mat3 &inertia_tensor);
    MathX::Mat3 GetInertiaTensor() const;
    void GetInertiaTensorWorld(MathX::Mat3 *inertia_tensor) const;
    MathX::Mat3 GetInertiaTensorWorld() const;
    void SetInverseInertiaTensor(const MathX::Mat3 &inverse_inertia_tensor);
    void GetInverseInertiaTensor(MathX::Mat3 *inverse_inertia_tensor) const;
    MathX::Mat3 GetInverseInertiaTensor() const;
    void GetInverseInertiaTensorWorld(MathX::Mat3 *inverse_inertia_tensor) const;
    MathX::Mat3 GetInverseInertiaTensorWorld() const;
    void SetDamping(const Real linear_damping, const Real angular_damping);
    void SetLinearDamping(const Real linear_damping);
    Real GetLinearDamping() const;
  };

}