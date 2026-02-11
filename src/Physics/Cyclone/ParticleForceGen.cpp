#include "Physics/Cyclone/ParticleForceGen.h"

namespace Cyc
{
  void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator *fg) {
    ParticleForceRegistration registration{particle,fg};
    m_registrations.push_back(registration);
    //m_registrations.emplace_back(particle,fg); //should be the same thing
  }

  void ParticleForceRegistry::Remove(Particle* particle, ParticleForceGenerator *fg) {
    
  }

  void ParticleForceRegistry::Clear() {
    
  }

  void ParticleForceRegistry::UpdateForces(MathX::Real time_step) {
    for (auto& r : m_registrations) {
      r.fg->UpdateForce(r.particle, time_step);
    }
  }

  void ParticleGravity::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    if (!particle->HasFiniteMass()) 
      return;
    MathX::Vec3 force = m_gravity * particle->GetMass();
    // particle->AddForce(m_gravity * particle->GetMass());
    particle->AddForce(force);
  }

  void ParticleDrag::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    MathX::Vec3 force = particle->GetVelocity();
    MathX::Real drag_coeff = glm::length(force);
    drag_coeff = m_k1 * drag_coeff + m_k2 * drag_coeff * drag_coeff;
    force = glm::normalize(force);
    force *= - drag_coeff;
    particle->AddForce(force);
  }

  void ParticleSpring::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    MathX::Vec3 force = particle->GetPosition();
    force -= m_other->GetPosition();
    MathX::Real magnitude = glm::length(force);
    magnitude = glm::abs(magnitude - m_rest_length);
    magnitude *= m_spring_const;
    force = glm::normalize(force);
    force *= - magnitude;
    particle->AddForce(force);
  }

  void ParticleAnchoredSpring::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    MathX::Vec3 force = particle->GetPosition();
    force -= *m_anchor;
    MathX::Real magnitude = glm::length(force);
    magnitude = (m_rest_length - magnitude) * m_spring_const;
    force = glm::normalize(force);
    force *= - magnitude;
    particle->AddForce(force);
  }

  void ParticleFakeSpring::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    if (!particle->HasFiniteMass()) 
      return;
    auto pos = particle->GetPosition();
    pos -= *m_anchor;

    // Calculate the constants and check they are in bounds.
    auto gamma = 0.5 * glm::sqrt(4*m_spring_const - m_damping*m_damping);
    if(gamma == 0.0)
      return;
    auto c = pos * (m_damping / (2.0*gamma)) + particle->GetVelocity()*(1.0/gamma) ;

    // Calculate the target position
    auto target = pos * glm::cos(gamma * time_step) + c * glm::sin(gamma * time_step);
    target *= glm::exp(-0.5 * time_step * m_damping);

    // Calculate the resulting acceleration and therefore the force
    auto accel = (target - pos) * (1.0 / (time_step*time_step)) -
        particle->GetVelocity() * (1.0/time_step);

    MathX::Vec3 force = accel * particle->GetMass();     
    // particle->AddForce(accel * particle->GetMass());
    particle->AddForce(force);
  }

  void ParticleBungee::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    MathX::Vec3 force = particle->GetPosition();
    force -= m_other->GetPosition();
    MathX::Real magnitude = glm::length(force);
    if (magnitude < m_rest_length)
      return;
    magnitude = m_spring_const * (m_rest_length - magnitude);
    force = glm::normalize(force);
    force *= - magnitude;
    particle->AddForce(force);
  }

  void ParticleAnchoredBungee::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    MathX::Vec3 force = particle->GetPosition();
    force -= *m_anchor;
    MathX::Real magnitude = glm::length(force);
    if (magnitude < m_rest_length) 
      return;
    magnitude = magnitude - m_rest_length;
    magnitude *= m_spring_const;
    force = glm::normalize(force);
    force *= - magnitude;
    particle->AddForce(force);
  }

  void ParticleBuoyancy::UpdateForce(Particle* particle, MathX::Real time_step)
  {
    MathX::Real depth = particle->GetPosition().y;
    if (depth >= m_water_height + m_max_depth) 
      return;
    MathX::Vec3 force{0,0,0};

    // Check if we're at maximum depth
    if (depth <= m_water_height - m_max_depth)
    {
        force.y = m_liquid_density * m_volume;
        particle->AddForce(force);
        return;
    }
    // Otherwise we are partly submerged
    force.y = m_liquid_density * m_volume *
        (depth - m_max_depth - m_water_height) / (2 * m_max_depth);
    particle->AddForce(force);
  }



}