#pragma once

#include "MathLib/MathLib.h"
#include "CoreLib/Core.h"

namespace Cyc
{
  class RigidBidy
  {

  protected:
    SpgMth::Real m_inverse_mass;
    SpgMth::Mat3 m_inverse_inertia_tensor; // In body space
    SpgMth::Real m_linear_damping;
    SpgMth::Real m_angular_damping;
    SpgMth::Vec3 m_position; //world space
    SpgMth::Quat m_orientation; // angular orientation in world space
    SpgMth::Vec3 m_velocity; //world space
    SpgMth::Vec3 m_rotation; //angular velocity in world space
    SpgMth::Mat3 m_inverse_inertia_tensor_world;
    SpgMth::Real m_motion; // use to put a body to 'sleep'
    bool m_is_awake;
    bool m_can_sleep;
    SpgMth::Mat4 m_transform_matrix; // body to world
    SpgMth::Vec3 m_force_accumulation;
    SpgMth::Vec3 m_torque_accumulation;
    SpgMth::Vec3 m_acceleration;
    SpgMth::Vec3 m_last_frame_acceleration;

  public:
    void CalculateDerivedData();
    void Integrate(SpgMth::Real time_step);
    void SetMass(const SpgMth::Real mass);
    SpgMth::Real GetMass() const;
    void SetInverseMass(const SpgMth::Real inverse_mass);
    SpgMth::Real GetInverseMass() const;
    bool HasFiniteMass() const;
    void setInertiaTensor(const SpgMth::Mat3 &inertia_tensor);
    SpgMth::Mat3 GetInertiaTensor() const;
    void GetInertiaTensorWorld(SpgMth::Mat3 *inertia_tensor) const;
    SpgMth::Mat3 GetInertiaTensorWorld() const;
    void SetInverseInertiaTensor(const SpgMth::Mat3 &inverse_inertia_tensor);
    void GetInverseInertiaTensor(SpgMth::Mat3 *inverse_inertia_tensor) const;
    SpgMth::Mat3 GetInverseInertiaTensor() const;
    void GetInverseInertiaTensorWorld(SpgMth::Mat3 *inverse_inertia_tensor) const;
    SpgMth::Mat3 GetInverseInertiaTensorWorld() const;
    void SetDamping(const SpgMth::Real linear_damping, const SpgMth::Real angular_damping);
    void SetLinearDamping(const SpgMth::Real linear_damping);
    SpgMth::Real GetLinearDamping() const;
  };

}