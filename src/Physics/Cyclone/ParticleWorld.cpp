#include "Physics/Cyclone/ParticleWorld.h"

namespace Cyc
{
  ParticleWorld::ParticleWorld(uint32_t max_contacts, uint32_t iterations) :
    m_resolver{iterations}, m_max_contacts{max_contacts}
  {
    m_contacts = new ParticleContact[max_contacts];
    m_calculate_iterations = (iterations == 0);
  }

  ParticleWorld::~ParticleWorld()
  {
    delete[] m_contacts;
  }

  uint32_t ParticleWorld::GenerateContacts()
  {
    uint32_t limit = m_max_contacts;
    ParticleContact* next_contact = m_contacts;

    for(auto g : m_contact_generators) {
      uint32_t used = g->AddContact(next_contact,limit);
      limit -= used;
      next_contact += used;

      // We've run out of contacts to fill. This means we're missing contacts.
      if(limit <=0)
        break;
    }

    // Return the number of contacts used.
    return m_max_contacts - limit;
  }

  void ParticleWorld::Integrate(mx::Real time_step)
  {
    for(auto* p : m_particles) {
      p->Integrate(time_step);
    }
  }

  void ParticleWorld::RunPhysics(mx::Real time_step)
  {
    m_registry.UpdateForces(time_step);
    Integrate(time_step);
    uint32_t used_contacts = GenerateContacts();
    if(used_contacts > 0) {
      if(m_calculate_iterations) {
        m_resolver.SetIterations(used_contacts * 2);
        m_resolver.ResolveContacts(m_contacts,used_contacts,time_step);
      }
    }
  }

  void ParticleWorld::StartFrame()
  {
    for(auto* p : m_particles) {
      p->ClearAccululator();
    }
  }

  ParticleWorld::Particles& ParticleWorld::GetParticles()
  {
    return m_particles;
  }

  ParticleWorld::ContactGenerators& ParticleWorld::GetContactGenerators()
  {
    return m_contact_generators;
  }

  ParticleForceRegistry& ParticleWorld::GetForceRegistry()
  {
    return m_registry;
  }

  void GroundContacts::Init(ParticleWorld::Particles* particles)
  {
    m_particles = particles;
  }

  uint32_t GroundContacts::AddContact(ParticleContact *contact,
     uint32_t limit) const {

    uint32_t count = 0;  
    for(auto* p : *m_particles) {
      mx::Real y = p->GetPosition().y;
      if(y < 0) {
        contact->contact_normal = mx::Vec3{0,1,0}; //Up
        contact->particle[0] = p;
        contact->particle[1] = nullptr;
        contact->penetration = -y;
        contact->restitution = 0.2;
        contact++;
        count++;  
      }
      if(count >= limit)
        return count;
    }
    return count; 
  }
}