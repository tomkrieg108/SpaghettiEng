#pragma once

#include "Physics/ParticleContacts.h"

namespace Phys
{
  class ParticleLink : ParticleContactGenerator
  {
    public:

      // Geneates the contacts to keep this link from being violated.
      virtual unsigned AddContact(ParticleContact *contact, uint32_t limit) const = 0;

      // Holds the pair of particles that are connected by this link.
      Particle* particle[2];
    protected:
      // Returns the current length of the link.
      mx::Real CurrentLength() const;
    private:

  };

  class ParticleCable : ParticleLink
  {
    public:
      mx::Real max_length;
      mx::Real restitution;

      virtual unsigned AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };

  class ParticleRod : ParticleLink
  {
    public:
      mx::Real length;
  
      virtual unsigned AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };

  class ParticleConstraint : ParticleContactGenerator
  {
    public:
      Particle* particle;
      mx::Vec3 anchor;

      virtual unsigned AddContact(ParticleContact *contact, uint32_t limit) const = 0;

    protected:
       mx::Real CurrentLength() const;    
  };

  class ParticleCableConstraint : public ParticleConstraint
  {
    public:
      mx::Real max_length;
      mx::Real restitution;
       virtual unsigned AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };

  class ParticleRodConstraint : public ParticleConstraint
  {
    public:
      mx::Real length;
      virtual unsigned AddContact(ParticleContact *contact, uint32_t limit) const override = 0;
  };


}