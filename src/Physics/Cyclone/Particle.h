#pragma once

#include "Math/MathX.h"
#include "Common/Common.h"

namespace Cyc
{
  using namespace MathX;

  struct Particle
  {
    public:
      void SetMass(MathX::Real mass) {
        SPG_ASSERT(mass > 0)
        m_inverse_mass = 1/mass;
      }

      MathX::Real GetMass() const noexcept {
        return m_inverse_mass == 0 ? MathX::REAL_MAX : 1.0/m_inverse_mass;
      }

      MathX::Vec3 GetPosition() const noexcept {
        return m_position;
      }

      MathX::Vec3 GetVelocity() const noexcept {
        return m_velocity;
      }

      MathX::Vec3 GetAcceleration() const noexcept {
        return m_acceleration;
      }

      MathX::Real GetInverseMass() const noexcept {
        return m_inverse_mass;
      }

      void AddForce(MathX::Vec3& force) noexcept {
        m_force_accumulator += force;
      }

      bool HasFiniteMass() const noexcept {
        return m_inverse_mass > 0;
      }

      void SetVelocity(MathX::Vec3 const& velocity) {
        m_velocity = velocity;
      }

      void ClearAccululator() {
        m_force_accumulator = {0,0,0};
      }

      void Integrate(MathX::Real time_step);

    public: 
      // Leave as public for now!  
      MathX::Real m_inverse_mass = 1.0;
      MathX::Vec3 m_position{0,0,0};
      MathX::Vec3 m_velocity{0,0,0};
      MathX::Vec3 m_acceleration{0,0,0};
      MathX::Vec3 m_force_accumulator{0,0,0};
      MathX::Real m_damping = 0.99; //simulate drag 
  };
}