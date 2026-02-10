#include "Physics/BallisticsDemo.h"

namespace Phys
{
  void BallisticsDemo::OnUpdate(MathX::Real time_step) {
    m_particle.Integrate(time_step);
  }

  void BallisticsDemo::SetParameters() {
    m_particle.SetMass(1.0);
    m_particle.m_velocity = {0.0, 0.0, 10.0f}; // 10m/s
    m_particle.m_acceleration = {0.0, 0.6, 0.0}; //floats up
    m_particle.m_damping = 0.9;
    m_particle.m_position = {0.0,1.5,0.0};
    m_particle.m_force_accumulator = {0,0,0};
  }
}