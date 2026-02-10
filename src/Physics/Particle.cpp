#include "Physics/Particle.h"

namespace Phys
{
  void Particle::Integrate(MathX::Real time_step) {

    if(m_inverse_mass <= 0)
      return;

    SPG_ASSERT(time_step > 0);
    
    m_position += m_velocity * time_step;

    MathX::Vec3 new_acceleration = m_acceleration + m_force_accumulator * m_inverse_mass;

    m_velocity += new_acceleration * time_step;
    m_velocity *= m_damping;

    m_force_accumulator = {0,0,0};
  }

}
