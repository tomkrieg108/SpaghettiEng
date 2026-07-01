#pragma once

#include "MathLib/MathLib.h"
#include "CoreLib/Core.h"

namespace Cyc
{
 

  struct Particle
  {
    public:
      void SetMass(SpgMth::Real mass) {
        SPG_ASSERT(mass > 0)
        m_inverse_mass = 1/mass;
      }

      SpgMth::Real GetMass() const noexcept {
        return m_inverse_mass == 0 ? SpgMth::REAL_MAX : 1.0/m_inverse_mass;
      }

      glm::vec3 GetPosition() const noexcept {
        return m_position;
      }

      glm::vec3 GetVelocity() const noexcept {
        return m_velocity;
      }

      glm::vec3 GetAcceleration() const noexcept {
        return m_acceleration;
      }

      SpgMth::Real GetInverseMass() const noexcept {
        return m_inverse_mass;
      }

      void AddForce(glm::vec3& force) noexcept {
        m_force_accumulator += force;
      }

      bool HasFiniteMass() const noexcept {
        return m_inverse_mass > 0;
      }

      void SetVelocity(glm::vec3 const& velocity) {
        m_velocity = velocity;
      }

      void ClearAccululator() {
        m_force_accumulator = {0,0,0};
      }

      void Integrate(SpgMth::Real time_step);

    public: 
      // Leave as public for now!  
      SpgMth::Real m_inverse_mass = 1.0;
      glm::vec3 m_position{0,0,0};
      glm::vec3 m_velocity{0,0,0};
      glm::vec3 m_acceleration{0,0,0};
      glm::vec3 m_force_accumulator{0,0,0};
      SpgMth::Real m_damping = 0.99; //simulate drag 
  };
}