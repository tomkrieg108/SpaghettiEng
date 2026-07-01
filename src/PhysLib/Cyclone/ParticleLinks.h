#pragma once

#include "PhysLib/Cyclone/ParticleContacts.h"

namespace Cyc
{
  
  class ParticleLink : ParticleContactGenerator
  {
    public:

      // Geneates the contacts to keep this link from being violated.
      virtual uint32_t AddContact(ParticleContact *contact, uint32_t limit) const = 0;

      // Holds the pair of particles that are connected by this link.
      Particle* m_particle[2];
    protected:
      // Returns the current length of the link.
      SpgMth::Real CurrentLength() const;
    private:

  };

  class ParticleCable : ParticleLink
  {
    public:
      SpgMth::Real m_max_length;
      SpgMth::Real m_restitution;

      virtual uint32_t AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };

  class ParticleRod : ParticleLink
  {
    public:
      SpgMth::Real m_length;
  
      virtual uint32_t AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };

  class ParticleConstraint : ParticleContactGenerator
  {
    public:
      Particle* m_particle;
      glm::vec3 m_anchor;

      virtual uint32_t AddContact(ParticleContact *contact, uint32_t limit) const = 0;

    protected:
       SpgMth::Real CurrentLength() const;    
  };

  class ParticleCableConstraint : public ParticleConstraint
  {
    public:
      SpgMth::Real m_max_length;
      SpgMth::Real m_restitution;
       virtual uint32_t AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };

  class ParticleRodConstraint : public ParticleConstraint
  {
    public:
      SpgMth::Real m_length;
      virtual uint32_t AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };


}