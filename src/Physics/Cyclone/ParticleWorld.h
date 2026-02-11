#pragma once

#include "Physics/Cyclone/ParticleForceGen.h"
#include "Physics/Cyclone/ParticleLinks.h"
#include "Math/MathX.h"

namespace Cyc
{
  class ParticleWorld
  {
    friend class GroundContacts;

    using Particles = std::vector<Particle*>;
    using ContactGenerators = std::vector<ParticleContactGenerator*>;
  public:

    ParticleWorld(uint32_t max_contacts, uint32_t iterations=0);
    ~ParticleWorld();
    uint32_t GenerateContacts();
    void Integrate(mx::Real time_step);
    void RunPhysics(mx::Real time_step);
    void StartFrame();
    Particles& GetParticles();
    ContactGenerators& GetContactGenerators();
    ParticleForceRegistry& GetForceRegistry();

  protected:
    Particles m_particles;

    // True if the world should calculate the number of iterations to give the contact resolver at each frame.
    bool m_calculate_iterations; 

    ParticleForceRegistry m_registry;
    ParticleContactResolver m_resolver;
    ContactGenerators m_contact_generators;
    ParticleContact* m_contacts; // Hold the list of contacts

    uint32_t m_max_contacts;
    
  private:

  };

  class GroundContacts : ParticleContactGenerator
  {
    public:
      void Init(ParticleWorld::Particles* particles);
      virtual uint32_t AddContact(ParticleContact *contact,
            uint32_t limit) const;

    private:
      ParticleWorld::Particles* m_particles;
  };

}