#include "PhysLib/Cyclone/ParticleLinks.h"
#include "PhysLib/Cyclone/Particle.h"

namespace Cyc
{
  SpgMth::Real ParticleLink::CurrentLength() const {
    SpgMth::Vec3 relative_pos = m_particle[0]->GetPosition() - 
        m_particle[1]->GetPosition();
    return glm::length(relative_pos);
  }

  uint32_t ParticleCable::AddContact(ParticleContact *contact, uint32_t limit) const {
    // Find the length of the cable
    SpgMth::Real length = CurrentLength();

    // Check if we're over-extended
    if (length < m_max_length)
        return 0;
    
    // Otherwise return the contact
    contact->particle[0] = m_particle[0];
    contact->particle[1] = m_particle[1];

    // Calculate the normal
    SpgMth::Vec3 normal = m_particle[1]->GetPosition() - m_particle[0]->GetPosition();
    normal = glm::normalize(normal);
  
    contact->contact_normal = normal;

    contact->penetration = length - m_max_length;
    contact->restitution = m_restitution;

    return 1;
  }

  uint32_t ParticleRod::AddContact(ParticleContact *contact, uint32_t limit) const {
    // Find the length of the cable
    SpgMth::Real length = CurrentLength();

    // Check if we're over-extended
    if (length < m_length)
        return 0;
    
    // Otherwise return the contact
    contact->particle[0] = m_particle[0];
    contact->particle[1] = m_particle[1];

    // Calculate the normal
    SpgMth::Vec3 normal = m_particle[1]->GetPosition() - m_particle[0]->GetPosition();
    normal = glm::normalize(normal);

    // The contact normal depends on whether we're extending or compressing
    if(length > m_length) {
      contact->contact_normal = normal;
      contact->penetration = length - m_length;
    }
    else {
      contact->contact_normal = -normal;
      contact->penetration = m_length - length;
    }
    
    // Always use zero restitution (no bounciness)
    contact->restitution = 0;

    return 1;                                 
  } 

  SpgMth::Real ParticleConstraint::CurrentLength() const {
    SpgMth::Vec3 relative_pos = m_particle->GetPosition() - m_anchor;
    return glm::length(relative_pos);
  }

  uint32_t ParticleCableConstraint::AddContact(ParticleContact *contact, uint32_t limit) const {
    // Find the length of the cable
    SpgMth::Real length = CurrentLength();

    // Check if we're over-extended
    if (length < m_max_length)
        return 0;
    
    // Otherwise return the contact
    contact->particle[0] = m_particle;
    contact->particle[1] = nullptr;

    // Calculate the normal
    SpgMth::Vec3 normal = m_particle->GetPosition();
    normal = glm::normalize(normal);
    contact->contact_normal = normal;

    contact->penetration = length - m_max_length;
    contact->restitution = m_restitution;

    return 1;
  }

  uint32_t ParticleRodConstraint::AddContact(ParticleContact *contact, uint32_t limit) const {
    // Find the length of the cable
    SpgMth::Real length = CurrentLength();

    // Check if we're over-extended
    if (length < m_length)
        return 0;
    
    // Otherwise return the contact
    contact->particle[0] = m_particle;
    contact->particle[1] = nullptr;

    // Calculate the normal
    SpgMth::Vec3 normal = m_anchor - m_particle->GetPosition();
    normal = glm::normalize(normal);

    // The contact normal depends on whether we're extending or compressing
    if(length > m_length) {
      contact->contact_normal = normal;
      contact->penetration = length - m_length;
    }
    else {
      contact->contact_normal = -normal;
      contact->penetration = m_length - length;
    }

    // Always use zero restitution (no bounciness)
    contact->restitution = 0;

    return 1;
  }

}