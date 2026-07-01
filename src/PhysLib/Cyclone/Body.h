#pragma once

#include "MathLib/MathLib.h"
#include "CoreLib/Core.h"

namespace Cyc
{
  class RigidBidy
  {

  protected:
    SpgMth::Real m_inverse_mass;
    glm::mat3 m_inverse_inertia_tensor; // In body space
    SpgMth::Real m_linear_damping;
    SpgMth::Real m_angular_damping;
    glm::vec3 m_position; //world space
    glm::quat m_orientation; // angular orientation in world space
    glm::vec3 m_velocity; //world space
    glm::vec3 m_rotation; //angular velocity in world space
    glm::mat3 m_inverse_inertia_tensor_world;
    SpgMth::Real m_motion; // use to put a body to 'sleep'
    bool m_is_awake;
    bool m_can_sleep;
    glm::mat4 m_transform_matrix; // body to world
    glm::vec3 m_force_accumulation;
    glm::vec3 m_torque_accumulation;
    glm::vec3 m_acceleration;
    glm::vec3 m_last_frame_acceleration;

  public:
    void CalculateDerivedData();
    void Integrate(SpgMth::Real time_step);
    void SetMass(const SpgMth::Real mass);
    SpgMth::Real GetMass() const;
    void SetInverseMass(const SpgMth::Real inverse_mass);
    SpgMth::Real GetInverseMass() const;
    bool HasFiniteMass() const;
    void setInertiaTensor(const glm::mat3 &inertia_tensor);
    glm::mat3 GetInertiaTensor() const;
    void GetInertiaTensorWorld(glm::mat3 *inertia_tensor) const;
    glm::mat3 GetInertiaTensorWorld() const;
    void SetInverseInertiaTensor(const glm::mat3 &inverse_inertia_tensor);
    void GetInverseInertiaTensor(glm::mat3 *inverse_inertia_tensor) const;
    glm::mat3 GetInverseInertiaTensor() const;
    void GetInverseInertiaTensorWorld(glm::mat3 *inverse_inertia_tensor) const;
    glm::mat3 GetInverseInertiaTensorWorld() const;
    void SetDamping(const SpgMth::Real linear_damping, const SpgMth::Real angular_damping);
    void SetLinearDamping(const SpgMth::Real linear_damping);
    SpgMth::Real GetLinearDamping() const;
  };

}